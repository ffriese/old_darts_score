#include "dart.h"
#include <QVector2D>
#include <QDebug>

Dart::Dart(QPointF _exactLocation, QPointF _intendedLocation, bool _generated, INTENT _intent){
    bust=false;
    generated = _generated;
    intent = _intent;
    exactLocation =_exactLocation;
    intendedLocation =_intendedLocation;
    location = DartBoard::getInstance()->getThrowResult(exactLocation);
    date = QDateTime::currentDateTime();
}

QPointF Dart::getExactLocation(){
    return exactLocation;
}

QPointF Dart::getIntendedLoctaion(){
    return intendedLocation;
}

QPair<int,DartBoard::BED> Dart::getLocation(){
    return location;
}

void Dart::editExactLocation(QPointF _exactLocation){
    exactLocation=_exactLocation;
    location=DartBoard::getInstance()->getThrowResult(exactLocation);
}

void Dart::editIntendedLocation(QPointF _intendedLocation){
    intendedLocation=_intendedLocation;
}

bool Dart::isBust(){
    return bust;
}

void Dart::setBust(bool _bust){
    bust=_bust;
}

QString Dart::getString(bool shortForm){
    if(exactLocation.x()==-1 && exactLocation.y()==-1){
        return "BO";
    }
    return DartBoard::getInstance()->getFieldName(location);
}

int Dart::getScore(){
    if(bust){
        return 0;
    }
    int score=DartBoard::getInstance()->calcScore(location);
    return score ;
}

double Dart::getDeviationCm(){
    if(intendedLocation.isNull()){
        return -1;
    }
    QVector2D distVec(exactLocation-intendedLocation);
    double dist = distVec.length()/10;
    return dist;
}

double Dart::getHDev(){
    if(intendedLocation.isNull()){
        return -1;
    }
    return std::abs(exactLocation.x()-intendedLocation.x());
}

double Dart::getVDev(){
    if(intendedLocation.isNull()){
        return -1;
    }
    return std::abs(exactLocation.y()-intendedLocation.y());
}
