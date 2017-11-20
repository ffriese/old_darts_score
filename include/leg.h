#ifndef LEG_H
#define LEG_H

#include <QObject>
#include "take.h"
#include "player.h"

class Leg
{
public:
    Leg(QSharedPointer<Player> beginner);

    void addTake(QSharedPointer<Take> take);

    QList<QSharedPointer<Take>> getTakes(){
        return takes;
    }

    QSharedPointer<Player> getBeginner(){
        return beginner;
    }

    QSharedPointer<Player> getWinner(){
        return winner;
    }

    void setWinner(QSharedPointer<Player> _winner){
        winner = _winner;
    }

private:
    int gameID;
    QSharedPointer<Player> beginner;
    QSharedPointer<Player> winner;
    QList<QSharedPointer<Take>> takes;
};

#endif // LEG_H
