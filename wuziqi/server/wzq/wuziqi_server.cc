
#include "wuziqi_server.h"
#include "s11/EventLoop.h"
#include<boost/noncopyable.hpp>
#include <sys/eventfd.h>


static int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    // LOG_SYSERR << "Failed in eventfd";
    abort();
  }
  return evtfd;
}

wuziqiServer::wuziqiServer(muduo::EventLoop* loop,muduo::TcpServer* ts):
loop_(loop),
overTime(createEventfd()),
overTimerChannel(new muduo::Channel(loop_,overTime))
{
    mperIsOn["client1"] = false;
    mperIsOn["client2"] = false;
    server = ts;

    server->setrcv6nameCallBack(boost::bind(&wuziqiServer::rcvIcd6FromClent,this,_1,_2,_3));

    overTimerChannel->setReadCallback(boost::bind(&wuziqiServer::timeOverCallBack,this));
    overTimerChannel->enableReading();
}




void wuziqiServer::rcvIcd1FromClient(const spPerson& p){
    if(mperWait.size()==0){
        mperWait[p->personId()] = p;
        p->state(person::perState::Waitting);
        
        //start the Timer;
    }else{
        MapPerson::iterator it=mperWait.begin();
        p->state(person::perState::Ingame);
        it->second->state(person::perState::Ingame);
        wpPerson pw = p;
        wpPerson pw1 = it->second; 
        spgame g(new game(pw,pw1,64));
        int t=g->rtGameId();
        mgame.insert({t,g});
        // wpgame gw = g;
        p->setRcvCallBack(boost::bind(&game::rcvIcd3FromPerson,g,_1));
        p->setGameId(g->rtGameId());
        it->second->setRcvCallBack(boost::bind(&game::rcvIcd3FromPerson,g,_1));
        it->second->setGameId(g->rtGameId());
        g->firstMove();
    }
    p->rtLoop()->runAfter(5000,boost::bind(&person::timeOverCallBack,p));
}


void wuziqiServer::freePerson(spPerson& s1,spPerson& s2){
    muduo::TcpConnectionPtr p = s1->rtconn();
    mperIing.erase(s1->personId());
    mperson.erase(s1->personId());        //free the person
    p->reDClose();                        //free the person's connection;

    mperNoth.insert({s2->personId(),s2});
    mperIing.erase(s2->personId());
    s2->state(person::perState::Nothing);     
}


void wuziqiServer::timeOverCallBack(){
    int read = 0;
    size_t n = ::read(overTime , &read,sizeof(read));  //rcv which person is overtime;
    if(n == 0){
        abort();
    }
    MapPerson::iterator it1      = mperson.find(read);  //find the person object
    int gid = it1->second->rtGameId();
    Mgame::iterator it  = mgame.find(gid);     //find the game object
    if(it!=mgame.end()){                       //end the game and free the overtime person,the other one turn to mperNoth;
        spgame p =mgame[gid];
        MgamePlayer m = p->rtGamePlayer();
        spPerson s1 = m[1].lock();
        spPerson s2 = m[2].lock();
        if(s1&&s2){
            if(s1->rtIsTimeOver()){
                freePerson(s1,s2);

            }else if(s2->rtIsTimeOver()){
                freePerson(s2,s1);
            }
        }
        mgame.erase(gid);
    }else{
        // abort();
        //maybe player1 and player2 dead at once;so the person maybe in mperNoth
        MapPerson::iterator it = mperNoth.find(read);
        if(it!=mperNoth.end()){
            spPerson sp=mperNoth[read];
            muduo::TcpConnectionPtr c = sp->rtconn();
            mperNoth.erase(read);
            mperson.erase(read);
            c->reDClose();
        }else{
            abort();
        }
    }
}

void wuziqiServer::rcvIcd6FromClent(const muduo::TcpConnectionPtr& conn,muduo::Buffer* buf, muduo::Timestamp){
    size_t len = buf->readableBytes();
    std::string str = buf->retrieveAsString();

    FileContents f;
    {
        std::istringstream is(str);
        boost::archive::binary_iarchive ia(is);
        ia>>f;
    }

    const std::string s = boost::lexical_cast<std::string>(f.payload);
    std::vector<std::string>v;
    int i=0,j=i;
    for(;i<s.size();i++){
        if(s[i]==' '){
            v.push_back(s.substr(j,i-j));
            j=i+1;
        }
    }
    int a=std::stoi(v[0]);
    if(a == 6){
        //mutex
        MapPersonOn::iterator it  = mperIsOn.find(v[1]);
        if(it == mperIsOn.end()){
            printf("wrong2");
        }else{
            if(mperIsOn[v[1]]){
                conn->reDClose();
            }else{
                mperIsOn[v[1]] = true;
                spPerson sp(
                    new person(conn->getLoop(),conn));
                sp->setRcv1CallBack(boost::bind(&wuziqiServer::rcvIcd1FromClient,this,_1));
                sp->setTimeOverId(overTime);
                conn->setMessageCallback(boost::bind(&person::recIcdFromClient,sp,_1,_2,_3));
                mperWait[sp->personId()] = sp;
                mperson[sp->personId()] = sp;
            }
        }

    }else{
        printf("wrong1");
    }
}