#pragma once

#include "datetime/Timestamp.h"
#include "s11/InetAddress.h"
#include "icd.h"
// #include "game.h"
#include "s11/Callbacks.h"
#include "callBack.h"
#include "s11/TimerId.h"
#include "s11/TcpConnection.h"

#include <string>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>




class Channel;
// class TimerId;
// class TcpConnection;

typedef boost::function<void(const dropPoint& )> Rcv3CallBack;



class person : boost::noncopyable,public boost::enable_shared_from_this<person>{
public:
    enum perState{Waitting,Ingame,Nothing};

    person(muduo::EventLoop* loop,const muduo::TcpConnectionPtr& conn);
    person(muduo::EventLoop* loop,const muduo::TcpConnectionPtr& conn,int id);
    // person(){}
    ~person(){}

    muduo::Timestamp waitTime(){return rcvTime_;}
    int personId(){return id_;}

    //receive the icd; and analyze the icd ,
    void recIcdFromClient(const muduo::TcpConnectionPtr& conn, muduo::Buffer* buf, muduo::Timestamp);

    //receive the icd from class game
    template<class A>
    void recIcdFromGameToClient(A& a){
        FileContents f({a});
        std::stringstream ss;
        boost::archive::binary_oarchive ia(ss);
        ia<<f;
        std::string s=ss.str();
        
        conn_->send(s+"\r\n");
    }

    void state(perState p){state_ = p;}
    //rcv from game send to Client: 02 03 05;

    void setRcvCallBack(const Rcv3CallBack& p)
    {
        rcv3CB = p;
    }
    void setRcv1CallBack(const Rcv1CallBack& cb)
    {rcv1CBInPerson =  cb; }
    
    muduo::TcpConnectionPtr rtconn(){
        return conn_;
    }

    void timeOverCallBack(){
        isOverTime = true;
        wakeup();
    }
    void wakeup();

    bool rtIsTimeOver(){
        return isOverTime;
    }

    void setTimeOverId(int t){
        timeOverId = t;
    }

    void setGameId(int t){
        gameId = t;
    }
    int rtGameId(){
        return gameId;
    }

    muduo::EventLoop* rtLoop(){
        return loop_;
    }


private:
    muduo::EventLoop* loop_;
    muduo::TimerId timeOver; 
    /*
        person's state , 
        perState must define before it be used ,so the function state must be defined after perState
    */    
    perState state_;
    

    static int kperIdCount;
    int id_;
    std::string name;
    muduo::Timestamp  rcvTime_;
    //boost::scoped_ptr<Channel> waitOverTime_;

    
    // wpgame game_;
    Rcv3CallBack rcv3CB;
    Rcv1CallBack rcv1CBInPerson;


    int gameId;
    bool isOverTime;
    int timeOverId;

    const muduo::TcpConnectionPtr conn_;
};

typedef boost::shared_ptr<person>   spPerson;
typedef boost::weak_ptr<person>     wpPerson;
typedef std::map<int,spPerson>     MapPerson;
typedef boost::function<void (const spPerson&)>         MapCalback;

