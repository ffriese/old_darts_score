#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "take.h"

class Player : public QObject
{
    Q_OBJECT
public:
    Player(int _id, QString _name, bool _ai, double _h_dev,double _v_dev, int _score=0);


    void change(int _id, QString _name, bool _ai, double _h_dev,double _v_dev);
    int getID();
    QString getName();
    bool isAi();
    double getHDev();
    void setHDev(double _h_dev);
    double getVDev();
    void setVDev(double _v_dev);

    int getScore();
    void setStartScore(int _startScore);
    int getTakeScore();
    int getDartsLeft();
    void setDartsLeft(int _dartsLeft);


    int getWonSets(){
        return wonSets;
    }
    int getWonLegs(){
        return wonLegs;
    }
    void setWonSets(int sets){
        wonSets = sets;
    }
    void setWonLegs(int legs){
        wonLegs = legs;
    }

   // void setWon(bool _won=true);
   // bool hasWon();


    double getLegStdDev();
    double getLegAvg();

    double getGameStdDev();
    double getGameAvg();

    QSharedPointer<Take> getCurrentTake();
    QList<QSharedPointer<Take>> getTakes();

    bool ownsTake(QSharedPointer<Take> _take);

    // X01-Games, maybe dont use
   // int getLiveScore(bool subtract=true);
    bool undoLastDart();
    int commitTake(Take::RESULT result=Take::NONE);

    void reset(int _startScore=0);
    void nextLeg(int _startScore=0);

private:
    int id;
    QString name;
    bool ai;
    double h_dev;
    double v_dev;
    int startScore;
    int dartsLeft;

    int wonLegs;
    int wonSets;
   // bool won;


    QList<QSharedPointer<Take>> gameTakes;

    QList<QSharedPointer<Take>> takes;
    QSharedPointer<Take> currentTake;

    void clearAll();

signals:

public slots:
};

#endif // PLAYER_H
