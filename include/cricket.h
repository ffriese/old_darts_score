#ifndef CRICKET_H
#define CRICKET_H


#include <QObject>
#include <QVector2D>
#include "game.h"

class Cricket : public Game
{
    Q_OBJECT
public:
    Cricket(int _legsToWin, QList<VARIANT> variants, int _gameID, int _setsToWin)
    : Game(_legsToWin, variants,_gameID, _setsToWin)
    {
        playerTurn = 0;
        int idx = 0;
        for(QSharedPointer<Player> player: players){
            player->reset((getStartScore()));
            QMap<int,int>* key = new QMap<int,int>();
            for(int f: CRICKET_FIELDS){
                key->insert(f,0);
            }
            HIT_COUNTER.insert(player,*key);
            if(idx==playerTurn){
                player->setDartsLeft(3);
            }
            idx++;
        }
    }
    void reset(){
        Game::reset();
        resetCounters();
    }

    void resetCounters(){

        HIT_COUNTER.clear();
        FIELD_CLOSED={false,false,false,false,false,false,false};
        for(QSharedPointer<Player> player: players){
            QMap<int,int>* key = new QMap<int,int>();
            for(int f: CRICKET_FIELDS){
                key->insert(f,0);
            }
            HIT_COUNTER.insert(player,*key);
        }
    }


    QMap<int,int> getHitCounter(QSharedPointer<Player> player){
        return HIT_COUNTER.value(player);
    }

    QString getGameName(){
        return "Cricket";
    }

    int getStartScore(){
        return 0;
    }

    int getWinner(){
        int idx =0;
        for(QSharedPointer<Player> player: players){
            if(player->getWonSets() == setsToWin){
                return idx;
            }
            idx++;
        }
        return -1;
    }

    int getLegWinner(){
         return -1; // TODO: IMPLEMENT REAL METHOD THAT DETERMINES IF A PLAYER HAS WON A LEG
    }

    QMap<QString,QList<Game::VARIANT>> getOptionGroups(){
        QMap<QString,QList<Game::VARIANT>> optionGroups;
        optionGroups.insert("MODE",{Game::CRICKET_NORMAL,Game::CRICKET_CUTTHROAT});
        return optionGroups;
    }

    bool isDefaultVariant(VARIANT v){
        switch(v){
        case CRICKET_NORMAL:
            return true;
        default:
            return false;
        }
    }

    Game::DARTRESULT handleDart(QSharedPointer<Dart> dart);

    void undoLastDart();

    int getLiveScore(QSharedPointer<Player> player);

    bool addDartToTake(QSharedPointer<Player> player, QSharedPointer<Dart> dart);

    void checkTurn();


    void init();
private:
    //int fieldHits(QSharedPointer<Player> player, int field);

    QList<VARIANT> acceptedVariants(){
        return {
            VARIANT::CRICKET_CUTTHROAT,
            VARIANT::CRICKET_NORMAL
        };
    }

    QList<HANDICAP> availableHandicaps(){
        return {};
    }
    void checkClosed(int num);

    QPair<int,DartBoard::BED> generateTarget();

    QMap<QSharedPointer<Player>,QMap<int,int>> HIT_COUNTER;
    QList<int> CRICKET_FIELDS={25,20,19,18,17,16,15};
    QList<bool> FIELD_CLOSED={false,false,false,false,false,false,false};

    QMap<QSharedPointer<Dart>,QString> significantDarts;


signals:

public slots:
};

#endif // CRICKET_H
