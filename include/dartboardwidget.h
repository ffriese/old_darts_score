#ifndef DARTBOARDWIDGET_H
#define DARTBOARDWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include "dartboard.h"
#include "dart.h"
#include "take.h"

class DartBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DartBoardWidget(QWidget *parent = 0);

private:

    DartBoard* dartBoard;
    double scale;
    QSharedPointer<Take> currentTake;

    QMap< QPair<int,DartBoard::BED> , QPainterPath* > paths;
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

    QPointF mouseCoordsToBoardCoords(int x, int y);

    QPointF intendedLoc;
    QFont wireFont;

signals:
    void newUserDart(QPointF loc, QPointF intendedLoc);
    void requestAIDart(QPointF intendedLoc);

public slots:
    void setTake(QSharedPointer<Take> take);
    void clearTake();
};

#endif // DARTBOARDWIDGET_H
