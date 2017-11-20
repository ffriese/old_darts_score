#ifndef ROUNDTHECLOCK_H
#define ROUNDTHECLOCK_H
#include "game.h"
#include <QDebug>

class RoundTheClock: public Game
{
public:
    RoundTheClock(int _legsToWin, QList<VARIANT> variants, int _gameID, int _setsToWin)
    : Game(_legsToWin, variants,_gameID, _setsToWin)
    {

        playerProgress.clear();
        for(QSharedPointer<Player> p: players){
            playerProgress.insert(p,0);
        }
    }

    int getLegWinner(){
        for(int i=0;i<players.count();i++){
            if(getLiveScore(players.at(i))>20){
                return i;
            }
        }
        return -1;
    }

    int getWinner(){
        for(int i=0;i<players.count();i++){
            if(players.at(i)->getWonSets() == setsToWin){
                return i;
            }
        }
        return -1;
    }
    int getStartScore(){
        return 0;
    }

    int getLiveScore(QSharedPointer<Player> player){
        return playerProgress.value(player);
    }

    QString getGameName(){
        return "Round The Clock";
    }

    QMap<QString, QList<VARIANT> > getOptionGroups(){
        QMap<QString,QList<Game::VARIANT>> optionGroups;
        return optionGroups;
    }

    bool isDefaultVariant(VARIANT v){
        return false;
    }

    QPair<int, DartBoard::BED> generateTarget(){
        int target = getLiveScore(getCurrentPlayer())+1;
        if(target>20){
            target = 25;
        }
        return QPair<int, DartBoard::BED>(target, target>20 ? DartBoard::BED::DOUBLE : DartBoard::BED::OUTER_SINGLE);
    }

    bool addDartToTake(QSharedPointer<Player> player, QSharedPointer<Dart> dart){
        if(dart->getLocation().first == playerProgress.value(player)+1){
            playerProgress.insert(player,playerProgress.value(player)+1);
        }
        if((dart->getLocation().first == 25 && playerProgress.value(player)==20)){
            playerProgress.insert(player,25);
        }
        QSharedPointer<Take> currentTake= player->getCurrentTake();
        currentTake->addDart(dart);
        player->setDartsLeft(player->getDartsLeft()-1);
        return true;

    }

    void checkTurn(){

        if(playerProgress.value(getCurrentPlayer())==25){

            getCurrentPlayer()->commitTake(Take::WIN);

            sets.last()->getCurrentLeg()->addTake(getCurrentPlayer()->getTakes().last());
            sets.last()->getCurrentLeg()->setWinner(getCurrentPlayer());

            getCurrentPlayer()->setWonLegs(getCurrentPlayer()->getWonLegs()+1);

            qDebug()<<getCurrentPlayer()->getName()<<"won the leg!"<<getCurrentPlayer()->getWonLegs()<<"of"<<legsToWin;

            for(QSharedPointer<Player> player: players){
                playerProgress.insert(player,0);
            }

            if(getCurrentPlayer()->getWonLegs() == legsToWin){
                getCurrentPlayer()->setWonSets(getCurrentPlayer()->getWonSets()+1);
                for(QSharedPointer<Player> player: players){
                    player->setWonLegs(0);
                }
                sets.last()->setWinner(getCurrentPlayer());


                qDebug()<<getCurrentPlayer()->getName()<<"won the set!"<<getCurrentPlayer()->getWonSets()<<"of"<<setsToWin;

                if(getCurrentPlayer()->getWonSets() == setsToWin){
                    status = FINNISHED;
                    qDebug()<<getCurrentPlayer()->getName()<<"won the game!";
                    //TODO: SET WINNER!!
                    return;
                }
            }

            status = LEG_FINNISHED;
            return;
        }

        if(getCurrentPlayer()->getDartsLeft()<1){
            // TURN END

            getCurrentPlayer()->commitTake(Take::NONE);
            sets.last()->getCurrentLeg()->addTake(getCurrentPlayer()->getTakes().last());


            if(playerTurn==players.count()-1){
               playerTurn=0;
            }else{
               playerTurn++;
            }

            getCurrentPlayer()->setDartsLeft(3);
        }
    }

private:
    QList<VARIANT> acceptedVariants(){
        return {};
    }

    QList<HANDICAP> availableHandicaps(){
        return {
            RTC_NO_L_SINGLE,
            RTC_NO_DOUBLE,
            RTC_NO_TRIPLE,
        };
    }

    QMap<QSharedPointer<Player>, int> playerProgress;
};

#endif // ROUNDTHECLOCK_H
