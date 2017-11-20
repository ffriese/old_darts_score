#ifndef SET_H
#define SET_H

#include <QObject>
#include "player.h"
#include "leg.h"

class Set
{
public:
    Set(QSharedPointer<Player> _beginner, int _legsToWin);

    QSharedPointer<Leg> addLeg(QSharedPointer<Player> _beginner);

    QSharedPointer<Player> getBeginner(){
        return beginner;
    }

    QSharedPointer<Player> getWinner(){
        return winner;
    }


    QList<QSharedPointer<Leg>> getLegs(){
        return legs;
    }

    void setWinner(QSharedPointer<Player> _winner){
        winner = _winner;
    }

    QSharedPointer<Leg> getCurrentLeg();
private:

    int gameID;
    int legsToWin;
    QSharedPointer<Player> beginner;
    QSharedPointer<Player> winner;
    QList<QSharedPointer<Leg>> legs;

};

#endif // SET_H
