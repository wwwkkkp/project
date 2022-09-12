#include "person.h"
#include "game.h"
#include "s11/Buffer.h"
#include "s11/EventLoop.h"


int person::kperIdCount = 0;



person::person(muduo::EventLoop* loop,const muduo::TcpConnectionPtr& conn):
    loop_(loop),
    id_(kperIdCount++),
    conn_(conn),
    state_(Nothing),
    isOverTime(false),
    gameId(0)
    // game_(NULL)
{
    // conn_ = conn;
}

person::person(muduo::EventLoop* loop ,const  muduo::TcpConnectionPtr& conn ,int id):
    loop_(loop),
    id_(id),
    conn_(conn),
    state_(Nothing),
    isOverTime(false),
    gameId(0)
    // game_(NULL)
{
    // conn_ = conn;
}

//receive the icd; and analyze the icd ,
void person::recIcdFromClient(const muduo::TcpConnectionPtr& conn, muduo::Buffer* buf, muduo::Timestamp){
    size_t len = buf->readableBytes();
    std::string s = buf->retrieveAsString();

    FileContents f;
    {
        std::istringstream is(s);
        boost::archive::binary_iarchive ia(is);
        ia>>f;
    }

    const std::string after = boost::lexical_cast<std::string>(f.payload);
    std::vector<int>v(10,0);
    for(int i=0,j=0,k=0;i<after.size();i++){
        if(after[i]==' '){
            v[k]=std::stoi(after.substr(j,i-j));
            j=i+1;
            k++;
        }
    }

    switch (*(icd*)&v[0]){
        case requese_:
            rcv1CBInPerson(shared_from_this());
        break;
        case dropPoint_:
            {
                dropPoint d(v[0],v[1],v[2],v[3],v[4]);
                rcv3CB(d);
            }
            break;
        case keepAlive_:
            // reset the timer;
            // loop_->runAfter(5000,boost::bind());
            loop_->cancel(timeOver);
            muduo::TimerId t=loop_->runAfter(5000,boost::bind(&person::timeOverCallBack,this));
            timeOver = t;
            break;
        // case reply_:
        //     //
        // break;


    }
}

void person::wakeup()
{
  uint64_t one = gameId;
  ssize_t n = ::write(timeOverId, &one, sizeof one);
  if (n != sizeof one)
  {
    // LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    abort();
  }
}
