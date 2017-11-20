#include "x01.h"
#include "utils.h"
#include <QDebug>

QPair<int, DartBoard::BED> X01::generateTarget(){
    int currScore = getLiveScore(getCurrentPlayer());

    if(variants.contains(X01_DOUBLE_IN) && currScore == getStartScore()){
        return QPair<int,DartBoard::BED>(20,DartBoard::DOUBLE);
    }

    if(variants.contains(X01_DOUBLE_OUT)){
        if(currScore <= 40 && (currScore % 2) == 0){
            return QPair<int, DartBoard::BED>(currScore/2, DartBoard::DOUBLE);
        }

        if(currScore < 100){
            for(int pref_d: PREFERRED_DOUBLES){
                int setup = currScore-(pref_d * 2);
                if(DartBoard::getInstance()->getAvailableScores().contains(setup)){
                     QList<QPair<int,DartBoard::BED> > options = DartBoard::getInstance()->getAvailableScores().values(setup);
                     // TODO: REAL CHECKOUT / CHOOSE EASIEST WAY
                     qDebug()<<options;
                     QPair<int,DartBoard::BED> best = options.first();
                     for(QPair<int,DartBoard::BED> option:options){
                         switch(option.second){
                         case DartBoard::BED::SINGLE:
                             return option;
                         case DartBoard::BED::TRIPLE:
                             best = option;
                         default:
                             break;
                         }
                     }
                     return best;
                }
                //qDebug()<<"found no setup for D"+QString::number(pref_d);
            }
        }
    }else{ //SINGLE OUT
        // NAIVE HEURISTIC: AIM AT HIGHEST POSSIBLE SCORE THAT DOES NOT BUST
        if(DartBoard::getInstance()->getAvailableScores().contains(currScore)){
            QList<QPair<int,DartBoard::BED> > options = DartBoard::getInstance()->getAvailableScores().values(currScore);

            for(QPair<int,DartBoard::BED> option:options){
                switch(option.second){
                case DartBoard::SINGLE:
                case DartBoard::OUTER_SINGLE:
                case DartBoard::INNER_SINGLE:
                    return option;
                default:
                    break;
                }

            }

            return DartBoard::getInstance()->getAvailableScores().value(currScore);
        }else if(currScore<60){
            int highestPossible=currScore;
            while(!DartBoard::getInstance()->getAvailableScores().contains(highestPossible)){
                highestPossible--;
            }
            return DartBoard::getInstance()->getAvailableScores().value(highestPossible);
        }
    }

    return QPair<int,DartBoard::BED>(20,DartBoard::TRIPLE);

}

void X01::checkTurn(){

    if(getLiveScore(getCurrentPlayer())==0){
        // WIN!!

        playScoreSound(getCurrentPlayer()->getCurrentTake()->getScore(),false);
        getCurrentPlayer()->commitTake(Take::WIN);

        sets.last()->getCurrentLeg()->addTake(getCurrentPlayer()->getTakes().last());
        sets.last()->getCurrentLeg()->setWinner(getCurrentPlayer());
        //players.at(playerTurn)->setWon();
        //gameActive=false;
        getCurrentPlayer()->setWonLegs(getCurrentPlayer()->getWonLegs()+1);


        qDebug()<<getCurrentPlayer()->getName()<<"won the leg!"<<getCurrentPlayer()->getWonLegs()<<"of"<<legsToWin;

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


        playScoreSound(getCurrentPlayer()->getCurrentTake()->getScore(),false);

        getCurrentPlayer()->commitTake(Take::NONE);
        sets.last()->getCurrentLeg()->addTake(getCurrentPlayer()->getTakes().last());


        if(playerTurn==players.count()-1){
           playerTurn=0;
        }else{
           playerTurn++;
        }

        getCurrentPlayer()->setDartsLeft(3);
     //   playScoreSound(getLiveScore(getCurrentPlayer()),true);
        qDebug()<<"---------------------------";
    }


}


bool X01::addDartToTake(QSharedPointer<Player> player, QSharedPointer<Dart> dart){

    playDartHitSound();
    QSharedPointer<Take> currentTake= player->getCurrentTake();

    int tmpScore =  player->getScore() - (dart->getScore()+currentTake->getScore());
    if((tmpScore == 0 && getVariants().contains(VARIANT::X01_DOUBLE_OUT) && dart->getLocation().second != DartBoard::BED::DOUBLE) ||
        (tmpScore == 1 && getVariants().contains(VARIANT::X01_DOUBLE_OUT))) {
             tmpScore = -1;
    }


    if(tmpScore < 0){
        //BUST
        dart->setBust();
        currentTake->addDart(dart);
      //  qDebug()<<name<<"busted!"<<"reset score to"<<getScore();
        player->setDartsLeft(0);
        return false;
    }else{
        currentTake->addDart(dart);
       // qDebug()<<"takescore: "<<currentTake->getScore();
        player->setDartsLeft(player->getDartsLeft()-1);
        return true;
    }

    return false;
}


void X01::init(){
    for(QSharedPointer<Player> player: players){
        player->reset((X*100)+1);
    }

}
