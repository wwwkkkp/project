#pragma once

#include "person.h"
#include "datetime/Timestamp.h"
// #include "callBack.h"

#include <vector>
#include <map>
#include <algorithm>
// #include <utility>
#include <boost/noncopyable.hpp>


class EventLoop;


typedef std::map<int,std::pair<int,int>> GameRecord;
typedef std::map<int,wpPerson>           MgamePlayer;

class game:boost::noncopyable{
public:
    enum roundPlayer{Player1 = 1,Player2};

    game(wpPerson playerOne,wpPerson playerTwo ,int boardSize ):
                                            // gamePalyers({NULL,playerOne,playerTwo}),
                                            gameId_(kgameIdInc_++),
                                            boardSize_(boardSize),
                                            gameBoard_(boardSize,std::vector<int>(boardSize,0))
    {
        gamePalyers[1] = playerOne;
        gamePalyers[2] = playerTwo;
        // gamePalyers.push_back(playerTwo);

    }
    game(wpPerson playerOne,wpPerson playerTwo ,int boardSize,int id):
                                            // gamePalyers({NULL,playerOne,playerTwo}),
                                            gameId_(id),
                                            boardSize_(boardSize),
                                            gameBoard_(boardSize,std::vector<int>(boardSize,0))
    {
        // gamePalyers(3,wpPerson);
        gamePalyers[1] = playerOne;
        gamePalyers[2] = playerTwo;
    }


    ~game(){}

    //who will play first; transfor by wuziqiserver;
    void firstMove();
    int nextMove();

    int directioncount(int z,int x,int y,int dx,int dy);
    //juice who win
    int whoWin();

    void sendIcd2ToClient(roundPlayer r,int isS,roundPlayer p,roundPlayer first);

    //interface function,receive the ICD from person ,juice if the right icd ,
    void rcvIcd3FromPerson(const dropPoint& d);
    //send icd 05 to player,transfor the corresponding function  in person 
    void sendIcd5ToClient(roundPlayer r,int isSu);

    //send class to person;
    template<class A>
    void sendIcdtoperson(roundPlayer r,A& a){
        wpPerson p = gamePalyers[*(int*)&r];
        spPerson sp = p.lock();
        if(sp)
            sp->recIcdFromGameToClient<A>(a);
    }

    int rtGameId(){
        return gameId_;
    }
    MgamePlayer rtGamePlayer(){
        return gamePalyers;
    }
    
private:

    static int kgameIdInc_;

    int gameId_;

    //the firstplayer and the secondplayer;
    roundPlayer firstPlayer_;
    roundPlayer secondePlayer_;
    //who will down next;
    roundPlayer roundState_;
    void state(roundPlayer r){ r = roundState_;}


    MgamePlayer gamePalyers;
    GameRecord gameRecord_;
    int gameNew;

    int boardSize_;
    std::vector<std::vector<int>>gameBoard_;

};

typedef boost::shared_ptr<game> spgame;
typedef std::map<int,spgame>    Mgame;
// typedef boost::weak_ptr<game> wpgame;

