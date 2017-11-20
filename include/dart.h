#ifndef DART_H
#define DART_H
#include "dartboard.h"
#include <QDateTime>

class Dart
{
public:
    Dart(QPointF _exactLocation,QPointF _intendedLocation=QPointF(), bool _generated=false);

    void setBust(bool _bust=true);

    QPointF getExactLocation();
    QPointF getIntendedLoctaion();
    QPair<int,DartBoard::BED> getLocation();

    void editExactLocation(QPointF _exactLocation);
    void editIntendedLocation(QPointF _intendedLocation);

    QString getString(bool shortForm=true);

    double getDeviationCm();
    double getHDev();
    double getVDev();
    int getScore();
    bool isBust();

    QString getDateString(){
        return date.date().toString("yyyy-MM-dd");
    }

    bool isGenerated(){
        return generated;
    }

private:
    QPair<int,DartBoard::BED> location;
    QPointF exactLocation;
    QPointF intendedLocation;
    QDateTime date;
    bool bust;
    bool generated;
};

#endif // DART_H
