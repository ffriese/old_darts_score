#include "leg.h"

Leg::Leg(QSharedPointer<Player> _beginner)
{
    beginner = _beginner;
    winner = QSharedPointer<Player>();
}

void Leg::addTake(QSharedPointer<Take> take){
    takes.append(take);
}
