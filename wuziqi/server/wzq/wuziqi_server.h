
// #include "person.h"
#include "game.h"
#include "s11/TcpServer.h"
#include "s11/Channel.h"
#include <vector>
// #include "callBack.h"

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>





class wuziqiServer: boost::noncopyable{
public:
    wuziqiServer(muduo::EventLoop* loop,muduo::TcpServer* ts);

    void rcvIcd1FromClient(const spPerson& p);

    void timeOverCallBack(); 

    void freePerson(spPerson& s1,spPerson& s2);

    void rcvIcd6FromClent(const muduo::TcpConnectionPtr& conn,muduo::Buffer* buf, muduo::Timestamp);
private:
    muduo::EventLoop* loop_;
    muduo::TcpServer* server;

    
    MapPerson    mperWait;
    MapPerson    mperIing;
    MapPerson    mperNoth;
    MapPerson    mperson;
    typedef std::map<std::string,bool> MapPersonOn;
    MapPersonOn mperIsOn;

    int overTime;
    boost::scoped_ptr<muduo::Channel> overTimerChannel;
    
    Mgame  mgame;

    


};