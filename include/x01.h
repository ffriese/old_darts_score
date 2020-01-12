#ifndef X01_H
#define X01_H

#include <QObject>
#include <QDebug>
#include "game.h"

class X01: public Game
{
public:
    X01(int _legsToWin, QList<VARIANT> variants, int _gameID, int _setsToWin)
        : Game(_legsToWin, variants,_gameID, _setsToWin)
    {
      if(variants.contains(X01_301)){
          X=3;
      }else if(variants.contains(X01_101)){
          X=1;
      }else if(variants.contains(X01_501)){
          X=5;
      }else if(variants.contains(X01_701)){
          X=7;
      }else if(variants.contains(X01_901)){
          X=9;
      }else{
          X=3;
      }
      if(variants.contains(X01_DOUBLE_OUT)){
          doubleOut = true;
          qDebug()<<"db_out detected!";
      }else{
          doubleOut = false;
          qDebug()<<"s_out!";
      }
      if(variants.contains(X01_DOUBLE_IN)){
          doubleIn = true;
      }else{
          doubleOut= false;
      }
    }

    QString getGameName(){
        return "X01";
    }

    int getStartScore(){
        return (X*100)+1;
    }

    int getLegWinner(){
        int idx =0;
        for(QSharedPointer<Player> player: players){
            if(getLiveScore(player) == 0){
                return idx;
            }
            idx++;
        }
        return -1;
    }

    int getWinner(){
        int idx =0;
        for(QSharedPointer<Player> player: players){
            if(player->getWonSets() == setsToWin){

                qDebug()<<"check for winner: "<<idx<<player->getWonSets() << setsToWin;
                return idx;
            }
            idx++;
        }
        //qDebug()<<"check for winner: "<<-1;
        return -1;
    }

    QMap<QString,QList<Game::VARIANT>> getOptionGroups(){
        QMap<QString,QList<Game::VARIANT>> optionGroups;
        optionGroups.insert("IN-MODE",{Game::X01_SINGLE_IN,Game::X01_MASTER_IN,Game::X01_DOUBLE_IN});
        optionGroups.insert("OUT-MODE",{Game::X01_SINGLE_OUT,Game::X01_MASTER_OUT,Game::X01_DOUBLE_OUT});
        return optionGroups;
    }

    bool isDefaultVariant(VARIANT v){
        switch(v){
        case X01_SINGLE_IN:
        case X01_SINGLE_OUT:
            return true;
        default:
            return false;
        }
    }


    void checkTurn();

    bool addDartToTake(QSharedPointer<Player> player, QSharedPointer<Dart> dart);


    void init();

private:
    int X;
    bool doubleOut;
    bool doubleIn;

    QList<VARIANT> acceptedVariants(){
        return {
            X01_SINGLE_OUT,
            X01_MASTER_OUT,
            X01_DOUBLE_OUT,
            X01_SINGLE_IN,
            X01_MASTER_IN,
            X01_DOUBLE_IN,
            X01_301,
            X01_501,
            X01_701,
            X01_901
        };
    }

    QList<HANDICAP> availableHandicaps(){
        return {
            X01_D_OUT,
            X01_T_OUT
        };
    }

    QPair<int,DartBoard::BED> generateTarget();

    QMap<int,QList<QPair<int, DartBoard::BED> > > checkouts;
    void generateFinishes();
};

#endif // X01_H
