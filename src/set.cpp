#include "set.h"

Set::Set(QSharedPointer<Player> _beginner, int _legsToWin)
{
    legsToWin = _legsToWin;
    beginner = _beginner;
    winner = QSharedPointer<Player>();
}

QSharedPointer<Leg> Set::addLeg(QSharedPointer<Player> _beginner){
    QSharedPointer<Leg> l = QSharedPointer<Leg>(new Leg(_beginner));
    legs.append(l);
    return l;
}

QSharedPointer<Leg> Set::getCurrentLeg(){
    if(!legs.isEmpty()){
        return legs.last();
    }
    return QSharedPointer<Leg>();
}

