#ifndef TAKE_H
#define TAKE_H

#include "dart.h"
#include <QSharedPointer>

class Take
{
public:
    Take();

    enum RESULT {UNFINISHED,NONE,BUST,WIN};


    QList<QSharedPointer<Dart> > getDarts();

    int getScore();
    double getAvgDeviation();
    RESULT getResult();

    bool addDart(QSharedPointer<Dart> dart);
    bool removeLastDart();
    void close(RESULT _result=NONE);



private:
    QList<QSharedPointer<Dart> >  darts;
    RESULT result;

};

#endif // TAKE_H
