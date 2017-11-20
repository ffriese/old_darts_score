#include "cricket.h"
#include <QDebug>

QPair<int, DartBoard::BED> Cricket::generateTarget(){

    qDebug()<<"TODO: implement good cricket target generator";
    int idx=0;
    foreach(bool closed, FIELD_CLOSED){
        if(!closed){
            if(CRICKET_FIELDS[idx]==25){
                return QPair<int,DartBoard::BED>(25,DartBoard::DOUBLE);
            }
            return QPair<int,DartBoard::BED>(CRICKET_FIELDS[idx],DartBoard::TRIPLE);
        }
        idx++;
    }
    return QPair<int,DartBoard::BED>(25,DartBoard::DOUBLE);
}

void Cricket::checkClosed(int num){
    if(!CRICKET_FIELDS.contains(num))
        return;
    for(QSharedPointer<Player> p : HIT_COUNTER.keys()){
        if(HIT_COUNTER.value(p).value(num)<3)
            return;
    }
    FIELD_CLOSED.replace(CRICKET_FIELDS.indexOf(num),true);
}

Game::DARTRESULT Cricket::handleDart(QSharedPointer<Dart> dart){

    qDebug()<<getCurrentPlayer()->getName()<<":"<<getCurrentPlayer()->getDartsLeft()<<"darts left"<<getCurrentPlayer().data();
    QPair<int,DartBoard::BED> result = DartBoard::getInstance()->getThrowResult(dart->getExactLocation());
    int score = DartBoard::getInstance()->calcScore(result);

    QString dbg_out;
    dbg_out =players.at(playerTurn)->getName()+" hit ";
    dbg_out.append(DartBoard::getInstance()->getFieldName(result));

    if(!dart->getIntendedLoctaion().isNull()){
        QVector2D distVec(dart->getExactLocation()-dart->getIntendedLoctaion());
        double dist = distVec.length()/10;
        dbg_out.append(" while aiming for ");
        dbg_out.append(DartBoard::getInstance()->getFieldName(DartBoard::getInstance()->getThrowResult(dart->getIntendedLoctaion())));
        dbg_out.append(", dist: ");
        dbg_out.append(QString::number(dist));
        dbg_out.append(" cm");
    }

    dbg_out.append(" -> ");
    dbg_out.append("value: "+QString::number(score));


    int number = result.first;

    int inc = result.second == DartBoard::BED::TRIPLE ? 3 : (result.second == DartBoard::BED::DOUBLE ? 2 : 1);
    if(CRICKET_FIELDS.contains(number)){
        while(inc>0){
            checkClosed(number);
            if(FIELD_CLOSED.at(CRICKET_FIELDS.indexOf(number))){
                qDebug()<<number<<"is already closed";
                break;
            }else{
                qDebug()<<number<<"added 1 to "<<number;
                QMap<int,int> hits = HIT_COUNTER.value(getCurrentPlayer());
                hits.insert(number,hits.value(number)+1);
                HIT_COUNTER.insert(getCurrentPlayer(),hits);
                inc--;
            }
        }
    }
    checkClosed(number);

    addDartToTake(players.at(playerTurn),dart);

    qDebug()<<dbg_out;


    checkTurn();

    switch(status){
    case LEG_FINNISHED:
    case FINNISHED:
        return DARTRESULT::WIN;
    default:
        return DARTRESULT::NONE;
    }

}

void Cricket::checkTurn(){

    bool allClosed = true;
    for(int field: CRICKET_FIELDS){
        if(HIT_COUNTER.value(getCurrentPlayer()).value(field)<3){
            allClosed = false;
            break;
        }
    }
    bool mostPoints = true;
    for(QSharedPointer<Player> p: getPlayers()){
        if(getLiveScore(p)>getLiveScore(getCurrentPlayer())){
            mostPoints = false;
            break;
        }
    }

    if(allClosed && mostPoints){
        // WIN!!
        getCurrentPlayer()->commitTake(Take::WIN);
        sets.last()->getCurrentLeg()->addTake(getCurrentPlayer()->getTakes().last());
        sets.last()->getCurrentLeg()->setWinner(getCurrentPlayer());
        //players.at(playerTurn)->setWon();
        //gameActive=false;
        getCurrentPlayer()->setWonLegs(getCurrentPlayer()->getWonLegs()+1);
        resetCounters();



        qDebug()<<getCurrentPlayer()->getName()<<"won the leg!"<<getCurrentPlayer()->getWonLegs()<<"of"<<legsToWin;

        if(getCurrentPlayer()->getWonLegs() == legsToWin){
            getCurrentPlayer()->setWonSets(getCurrentPlayer()->getWonSets()+1);
            for(QSharedPointer<Player> player: players){
                player->setWonLegs(0);
            }

            qDebug()<<getCurrentPlayer()->getName()<<"won the set!"<<getCurrentPlayer()->getWonSets()<<"of"<<setsToWin;

            if(getCurrentPlayer()->getWonSets() == setsToWin){
                status = FINNISHED;
                qDebug()<<getCurrentPlayer()->getName()<<"won the game!";
                //TODO: SET WINNER!!
                sets.last()->setWinner(getCurrentPlayer());
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
        qDebug()<<"---------------------------";
    }

}


int Cricket::getLiveScore(QSharedPointer<Player> player){
    QMap<int,int> hits = HIT_COUNTER.value(player);
    int score = 0;
    for(int field :hits.keys()){
        int num_hits = hits.value(field);
        if(num_hits>3){
            score+=(num_hits-3)*field;
        }
    }
    return score;
}

bool Cricket::addDartToTake(QSharedPointer<Player> player, QSharedPointer<Dart> dart){
    player->getCurrentTake()->addDart(dart);
    // qDebug()<<"takescore: "<<currentTake->getScore();
    player->setDartsLeft(player->getDartsLeft()-1);
    return true;
}

void Cricket::undoLastDart(){
    if(status != IN_PROGRESS){
        int winner = getWinner();
        if(winner<0){
            return;
        }
        playerTurn=winner;
        players.at(playerTurn)->undoLastDart();
        return;
    }
    int dartsLeft=players.at(playerTurn)->getDartsLeft();
    if(dartsLeft<3 && dartsLeft>0){
        players.at(playerTurn)->undoLastDart();
    }else if(dartsLeft==3){
        int prevIdx = playerTurn==0 ? prevIdx=players.count()-1 : playerTurn-1;
        if(players.at(prevIdx)->getTakes().count()>0){
            players.at(playerTurn)->setDartsLeft(0);
            playerTurn=prevIdx;
            players.at(playerTurn)->undoLastDart();
        }
    }
}

void Cricket::init(){
    for(QSharedPointer<Player> player: players){
        player->reset(0);
    }

}
