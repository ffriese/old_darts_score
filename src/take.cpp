#include "take.h"
#include <QDebug>

Take::Take()
{
 darts.clear();
 result = UNFINISHED;
}

int Take::getScore(){
    int score=0;
    for(QSharedPointer<Dart> dart:darts){
        if(!dart->isBust()){
            score+=dart->getScore();
        }else{
            return 0;
        }
    }
    return score;
}

bool Take::addDart(QSharedPointer<Dart> dart){
    if(darts.count()<3 && result == UNFINISHED){
        darts.append(dart);
        //darts.append(QSharedPointer<Dart>(dart));
        if(dart->isBust()){
            close(BUST);
        }
        return true;
    }
    return false;
}

bool Take::removeLastDart(){
    if(darts.count()>0){
       // delete darts.last();
        darts.removeLast();
        result=UNFINISHED;
        return true;
    }
    return false;
}

QList<QSharedPointer< Dart>> Take::getDarts(){
    return darts;
}

void Take::close(RESULT _result){
    result=_result;
}

Take::RESULT Take::getResult(){
    return result;
}

double Take::getAvgDeviation(){
    double dev=0.0;
    double cnt=0;
    for(QSharedPointer<Dart> dart:darts){
        if(!dart->getIntendedLoctaion().isNull()){
            dev+=dart->getScore();
            cnt++;
        }
    }
    if(cnt>0){
        return dev/cnt;
    }
    return -1;
}
