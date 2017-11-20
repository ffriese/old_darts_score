#include "player.h"
#include <QDebug>

Player::Player(int _id, QString _name, bool _ai, double _h_dev, double _v_dev, int _startScore){
    id = _id;
    name = _name;
    ai = _ai;
    h_dev = _h_dev;
    v_dev = _v_dev;
    startScore=_startScore;
    currentTake= QSharedPointer<Take>(new Take()); //TODO:;: MAYBE IGNORE
    takes.clear();
    dartsLeft=0;
    wonLegs = 0;
    wonSets = 0;
    qDebug()<<"newly instantiated: "<<name<<this;
}

void Player::change(int _id, QString _name, bool _ai, double _h_dev, double _v_dev){
    id = _id;
    name = _name;
    ai = _ai;
    h_dev = _h_dev;
    v_dev = _v_dev;
    qDebug()<<"playerChange: "<<name<<this;
    clearAll();
}

int Player::getID(){
    return id;
}

QString Player::getName(){
    return name;
}

bool Player::isAi(){
    return ai;
}

double Player::getHDev(){
    return h_dev;
}
void Player::setHDev(double _h_dev){
    h_dev=_h_dev;
}
double Player::getVDev(){
    return v_dev;
}
void Player::setVDev(double _v_dev){
    v_dev=_v_dev;
}

int Player::getScore(){

    int takeSum=0;
    for(QSharedPointer<Take> take:takes){
        takeSum+=take->getScore();
    }


    return startScore-takeSum;
}


void Player::setStartScore(int _startScore){
    startScore=_startScore;
}

int Player::getTakeScore(){
    return currentTake->getScore();
}


/*
int Player::getLiveScore(bool subtract){
    if(subtract){
        return getScore()-getTakeScore();
    }
    else{
        return getScore()+getTakeScore();
    }
}
*/

int Player::getDartsLeft(){
    return dartsLeft;
}

void Player::setDartsLeft(int _dartsLeft){
    qDebug()<<getName()<<" was set to "<<_dartsLeft<<"darts"<<this;
    dartsLeft=_dartsLeft;
}

QSharedPointer<Take> Player::getCurrentTake(){
    return currentTake;
}

QList<QSharedPointer<Take>> Player::getTakes(){
    return takes;
}


/*
void Player::setWon(bool _won){
    won=_won;
}

bool Player::hasWon(){
    return won;
}
*/


double Player::getLegStdDev(){
    double dev=0.0;
    double cnt=0;
    for(QSharedPointer<Take> t: takes){
        for (QSharedPointer<Dart> d: t->getDarts()){
            if(!d->getIntendedLoctaion().isNull()){
                dev+=d->getDeviationCm();
                cnt++;
            }
        }
    }
    for(QSharedPointer<Dart> d: currentTake->getDarts()){
        if(!d->getIntendedLoctaion().isNull()){
            dev+=d->getDeviationCm();
            cnt++;
        }
    }
    if(cnt>0)
        return dev/cnt;
    return 0.0;
}

double Player::getLegAvg(){
    double avg=0.0;
    double cnt=0;
    for(QSharedPointer<Take> t: takes){
        avg+=t->getScore();
        cnt+=3;
    }
    for(QSharedPointer<Dart> d: currentTake->getDarts()){
        avg+=d->getScore();
        cnt++;
    }
    if(cnt>0)
        return avg/cnt;
    return 0.0;
}

double Player::getGameStdDev(){
    double dev=0.0;
    double cnt=0;

    for(QSharedPointer<Take> t: gameTakes){
        for (QSharedPointer<Dart> d: t->getDarts()){
            if(!d->getIntendedLoctaion().isNull()){
                dev+=d->getDeviationCm();
                cnt++;
            }
        }
    }

    for(QSharedPointer<Take> t: takes){
        for (QSharedPointer<Dart> d: t->getDarts()){
            if(!d->getIntendedLoctaion().isNull()){
                dev+=d->getDeviationCm();
                cnt++;
            }
        }
    }
    for(QSharedPointer<Dart> d: currentTake->getDarts()){
        if(!d->getIntendedLoctaion().isNull()){
            dev+=d->getDeviationCm();
            cnt++;
        }
    }
    if(cnt>0)
        return dev/cnt;
    return 0.0;
}

double Player::getGameAvg(){
    double avg=0.0;
    double cnt=0;

    for(QSharedPointer<Take> t: gameTakes){
        avg+=t->getScore();
        cnt+=3;
    }

    for(QSharedPointer<Take> t: takes){
        avg+=t->getScore();
        cnt+=3;
    }
    for(QSharedPointer<Dart> d: currentTake->getDarts()){
        avg+=d->getScore();
        cnt++;
    }
    if(cnt>0)
        return avg/cnt;
    return 0.0;
}


bool Player::undoLastDart(){
    //TODO:: FIX THIS FOR TO SUPPORT ALL GAMETYPES
    if(this->getScore()-this->getTakeScore()==0){
        //won=false;
        //delete currentTake;
        currentTake=takes.last();
        takes.removeLast();
    }
    if(currentTake->removeLastDart()){
        setDartsLeft(3-currentTake->getDarts().count());
       return true;
    }else{
        if(takes.count()>0){
          //  delete currentTake;
            currentTake=takes.last();
            takes.removeLast();
            if(currentTake->removeLastDart()){
                setDartsLeft(3-currentTake->getDarts().count());
                return true;
            }
        }
    }
    return false;
}

bool Player::ownsTake(QSharedPointer<Take> _take){
    return gameTakes.contains(_take)||takes.contains(_take);
}

int Player::commitTake(Take::RESULT result){
    if(currentTake->getResult()==Take::UNFINISHED){
        currentTake->close(result);
    }
    //score-=currentTake->getScore();
    takes.append(currentTake);
    currentTake= QSharedPointer<Take>(new Take()); // TODO: MAYBE RESET TO NULL
    return getScore();
}

void Player::reset(int _startScore){
    clearAll();
    startScore=_startScore;
}

void Player::nextLeg(int _startScore){
    qDebug()<<this<<"reset score to"<<_startScore;
    startScore=_startScore;

    gameTakes.append(takes);

    takes.clear();
    currentTake= QSharedPointer<Take>(new Take()); //TODO: MAYBE RESET TO NULL
}

void Player::clearAll(){
    takes.clear();
    gameTakes.clear();
    currentTake= QSharedPointer<Take>(new Take()); //TODO: MAYBE RESET TO NULL

    qDebug()<<"player clear called for "<<this;
    wonLegs = 0;
    wonSets = 0;
    dartsLeft=0;
}
