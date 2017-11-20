#ifndef DARTBOARD_H
#define DARTBOARD_H

#include <QObject>
#include <QMap>
#include <QMultiMap>
#include <QPainterPath>

class DartBoard
{
public:
    DartBoard();

    enum BED {NONE,SINGLE,INNER_SINGLE,OUTER_SINGLE,DOUBLE,TRIPLE};

    static DartBoard* getInstance(){
            static DartBoard* instance;
            if(instance==NULL){
                instance = new DartBoard();
            }
            return instance;
    }

    QMap< QPair<int,BED> , QPainterPath* > getFields();
    QMultiMap<int,QPair<int,BED> > getAvailableScores();


    QPointF getCenterEstimate(QPair<int,BED> field, double std_dev=20.0);
    QPointF aimDartAt(QPointF intendedLoc, double h_dev, double v_dev);
    QPair<int,BED> getThrowResult(QPointF location);

    int calcScore(QPair<int,BED> hitField);
    QString getFieldName(QPair<int,BED> hitField,bool shortForm=true);

private:
    void initBoard();
    void makePartialPiePart(QPainterPath* path, int cx, int cy, double radius1, double radius2, double start_angle, double angle);

    QMap< QPair<int,BED> , QPainterPath* > fields;
    QMultiMap<int,QPair<int,BED> > availableScores;
};

#endif // DARTBOARD_H
