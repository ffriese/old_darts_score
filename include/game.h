#ifndef GAME_H
#define GAME_H

#include <QObject>
#include "dartboard.h"
#include "player.h"
#include "set.h"
#include "leg.h"
#include "dart.h"
#include <QSharedPointer>

class Game: public QObject
{
    Q_OBJECT
public:
    enum VARIANT{
        X01_301,X01_501,X01_701,X01_901,
        X01_SINGLE_OUT,X01_MASTER_OUT,X01_DOUBLE_OUT,
        X01_SINGLE_IN,X01_MASTER_IN,X01_DOUBLE_IN,
        CRICKET_NORMAL,CRICKET_CUTTHROAT
    };

    enum HANDICAP{
        X01_D_OUT, X01_T_OUT,
        RTC_NO_L_SINGLE, RTC_NO_TRIPLE, RTC_NO_DOUBLE
    };

    enum STATUS{
        WAITING_FOR_USER_INPUT,
        IN_PROGRESS,
        LEG_FINNISHED,
        FINNISHED,
        BLOCKING
    };

    enum DARTRESULT{
        NONE,WIN,TURNCHANGE
    };

protected:
    Game(int _legsToWin, QList<VARIANT> _variants,int _gameID = -1, int _setsToWin = 1);

public:

    virtual int getLiveScore(QSharedPointer<Player> player){
        return player->getScore() - player->getTakeScore();
    }

    virtual int getLegWinner() = 0;
    virtual int getWinner() = 0;
    virtual int getStartScore() = 0;
    virtual QString getGameName() = 0;

    virtual QMap<QString,QList<VARIANT>> getOptionGroups() = 0;
    virtual bool isDefaultVariant(VARIANT v) = 0;

    QList<VARIANT> getVariants(){
        return variants;
    }

    int getSetsToWin(){
        return setsToWin;
    }

    int getLegsToWin(){
        return legsToWin;
    }

    QList<QSharedPointer<Set>> getSets(){
        return sets;
    }

    QString getVariantStrings(){
        QString s="";
        for(int i = 0; i<variants.count(); i++){
            s = s + getVariantString(variants.at(i));
            if(i < variants.count()-1){
               s = s +", ";
            }
        }
        return s;
    }

    QString getVariantString(VARIANT v){

        switch(v){
        case X01_301:
            return "301";
        case X01_501:
            return "501";
        case X01_701:
            return "701";
        case X01_901:
            return "901";
        case X01_SINGLE_IN:
            return "Single In";
        case X01_SINGLE_OUT:
            return "Single Out";
        case X01_DOUBLE_IN:
            return "Double In";
        case X01_DOUBLE_OUT:
            return "Double Out";
        case X01_MASTER_IN:
            return "Master In";
        case X01_MASTER_OUT:
            return "Master OUT";
        case CRICKET_CUTTHROAT:
            return "Cut-throat";
        case CRICKET_NORMAL:
            return "Classic";
        default:
            return "";
        }
    }

    QList<QSharedPointer<Player>> getPlayers(){
        return players;
    }
    QSharedPointer<Player> getCurrentPlayer(){
        return players.at(playerTurn);
    }

    QSharedPointer<Player> getLastPlayer(){
        if(playerTurn==0)
            return players.last();
        return players.at(playerTurn-1);
    }

    QSharedPointer<Player> getCurrentLegBeginner(){
        if(!sets.isEmpty()){
            if(!sets.last()->getCurrentLeg().isNull())
                return sets.last()->getCurrentLeg()->getBeginner();
        }
        return QSharedPointer<Player>();
    }

    QSharedPointer<Player> getCurrentSetBeginner(){
        if(!sets.isEmpty())
            return sets.last()->getBeginner();
        return QSharedPointer<Player>();
    }




    int getPlayerTurn(){
        return playerTurn;
    }

    int getID(){
        return gameID;
    }


    void start(QList<QSharedPointer<Player>> _players,
               QMap<QSharedPointer<Player>,QList<HANDICAP> > _handicaps = QMap<QSharedPointer<Player>,QList<Game::HANDICAP>>(),
               int _startingPlayer = 0);

    void continueGame();

    virtual void reset(){
        playerTurn = 0;
        int idx = 0;
        for(QSharedPointer<Player> player: players){
            player->reset((getStartScore()));
            if(idx==playerTurn){
                player->setDartsLeft(3);
            }
            idx++;
        }
        if(players.count()>0){
            sets.clear();
            sets.append(QSharedPointer<Set>(new Set(players.first(),legsToWin)));
            sets.last()->addLeg(players.first());
        }
    }

    QSharedPointer<Player> findTakeOwner(QSharedPointer<Take> take){
        foreach(QSharedPointer<Player> p, players){
           if(p->ownsTake(take)){
               return p;
           }
        }
        return QSharedPointer<Player>();
    }


    virtual DARTRESULT handleDart(QSharedPointer<Dart> dart);

    virtual QPair<int,DartBoard::BED> generateTarget() = 0;

    virtual bool addDartToTake(QSharedPointer<Player> player, QSharedPointer<Dart> dart) = 0;

    STATUS getStatus(){
        return status;
    }


    void block(){
        if(status==BLOCKING){
            return;
        }
        tmp_status = status;
        status = BLOCKING;
    }

    void unblock(){
        status = tmp_status;
    }

    void undoLastDart();
    virtual void checkTurn() = 0;


    QList<HANDICAP> getAvailableHandicaps(){
        return availableHandicaps();
    }

private:

    int gameID;
    int legsToWin;
    int setsToWin;
    QList<VARIANT> variants;

    STATUS status;
    STATUS tmp_status;

    bool finished;
    int playerTurn;
    QList<QSharedPointer<Set>> sets;
    QList<QSharedPointer<Player>> players;
    QMap<QSharedPointer<Player>, QList<HANDICAP> > handicaps;

    void playScoreSound(int score, bool req);
    void playDartHitSound();

    void nextLeg();

    virtual void init(){}
    virtual QList<VARIANT> acceptedVariants() = 0;
    virtual QList<HANDICAP> availableHandicaps() = 0;

    int getNextPlayerIdx(int pIdx);

    friend class X01;
    friend class Cricket;
    friend class RoundTheClock;
};

#endif // GAME_H
