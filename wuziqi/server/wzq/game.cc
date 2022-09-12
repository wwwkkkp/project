#include "person.h"
#include "game.h"

int game::kgameIdInc_   =   0;


void game::firstMove(){
    int f=rand()%2;
    if(f){
        state(Player1);
        firstPlayer_    =   Player1;
        secondePlayer_  =   Player2;
    }
    else{
        state(Player2);
        firstPlayer_    =   Player2;
        secondePlayer_  =   Player1;

    }
    
    sendIcd2ToClient(firstPlayer_,1,Player1,firstPlayer_);
    sendIcd2ToClient(secondePlayer_,1,Player2,firstPlayer_);

}

int game::nextMove(){
    if(gameRecord_.size()){
        GameRecord::reverse_iterator it=gameRecord_.rbegin();
        if(it->first%2) return firstPlayer_;
        else return *(int*)&secondePlayer_;
    }
    else return *(int*)&firstPlayer_;
}


int game::directioncount(int z,int x,int y,int dx,int dy){
    int count_=0;
    for(int i=0;i<5;i++){
        int x1=x+dx;
        int y1=y+dy;
        if(x1<0||x1>=boardSize_||y1<0||y1>=boardSize_||gameBoard_[x1][y1]!=z){
            return count_;
        }else{
            count_++;
            x=x1;y=y1;
        }
    }
    return count_;
}

int game::whoWin(){
    if(gameRecord_.size() == 0)return 0;  //cannot sure who win
    GameRecord::reverse_iterator it = gameRecord_.rbegin();
    int x = it->second.first;
    int y = it->second.second;
    int a = it->first%2;
    if(a){
        gameBoard_[x][y] = 1;
    }else{
        gameBoard_[x][y] = 2;
    }
    int dx[]={-1,1,0,0,-1,1,1,-1};
    int dy[]={0,0,-1,1,1,-1,1,-1};

    int count_ = 1;
    for(int i=0;i<4;i++){
        count_ = std::max(count_,1+ directioncount(a+1,x,y,dx[i],dy[i]) + directioncount(a+1,x,y,dx[i+1],dy[i+1]));
    }
    if(count_>=5){
        return a+1;
    }

    return 0;
}

void game::sendIcd2ToClient(roundPlayer r,int isS,roundPlayer p,roundPlayer first){
    success s(2,isS,*(int*)&p,*(int*)&first);
    sendIcdtoperson<success>(r,s);
}


void game::rcvIcd3FromPerson(const dropPoint& d){
    if(d.player == nextMove()){
        for(GameRecord::iterator it = gameRecord_.begin();it!=gameRecord_.end();it++){
            int x1 = it->second.first;
            int y1 = it->second.second;
            if((x1 == d.x && y1 == d.y)||d.x<0||d.x>=boardSize_||d.y<0||d.y>=boardSize_){
                sendIcd5ToClient(*(roundPlayer*)&d.player,0);
                return;
            }
        }
        sendIcd5ToClient(*(roundPlayer*)&d.player,1);
        roundPlayer r = d.player == Player1? Player2 : Player1;
        sendIcdtoperson<const dropPoint>(r,d);
        gameRecord_.insert({gameNew++,{d.x,d.y}});
        int t = whoWin();
        if(t){
            ifWin w(6,1);
            ifWin f(6,0);
            roundPlayer p = *(roundPlayer*)&t;
            t=t%2 + 1;
            roundPlayer q = *(roundPlayer*)&(t);
            sendIcdtoperson<ifWin>(p,w);
            sendIcdtoperson<ifWin>(q,f);

            sleep(100);
            //closegame();
            spPerson s1 = gamePalyers[1].lock();
            if(s1){
                muduo::TcpConnectionPtr tp1= s1->rtconn();
                tp1->shutdown();
            }

            spPerson s2 = gamePalyers[2].lock();
            if(s2){
                muduo::TcpConnectionPtr tp2= s2->rtconn();
                tp2->shutdown();
            }
        }

            
    }else{
        sendIcd5ToClient(*(roundPlayer*)&d.player,0);
    }
}

void game::sendIcd5ToClient(roundPlayer r,int isSu){
    reply re(5,r,isSu);
    //send r to player r
    sendIcdtoperson<reply>(r,re);
}

