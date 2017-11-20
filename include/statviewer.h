#ifndef STATVIEWER_H
#define STATVIEWER_H

#include <QFrame>
#include "dbmanager.h"

namespace Ui {
class StatViewer;
}

class StatViewer : public QFrame
{
    Q_OBJECT

public:
    explicit StatViewer(QWidget *parent = 0);
    ~StatViewer();


private slots:
    void executeQuery();

private:
    Ui::StatViewer *ui;
    DBManager* db;
    QList<QSharedPointer<Player> > players;
    QMap<QString,QSharedPointer<Player> > playerMap;
};

#endif // STATVIEWER_H
