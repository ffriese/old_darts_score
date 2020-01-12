#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QLCDNumber>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QSignalMapper>
#include <QRadioButton>
#include <QMap>
#include <QActionGroup>
#include <QSharedPointer>
#include "dbmanager.h"
#include "gamemanager.h"
#include "player.h"
#include "playerwidget.h"
#include "statviewer.h"
//#include <ros/ros.h>
//#include <geometry_msgs/Point.h>
//#include <QtMqtt/QtMqtt>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void resizeEvent(QResizeEvent *);
    void cvDart(QPointF dart){emit newCvDart(dart, QPointF());}
    //void cvDart(const geometry_msgs::Point::ConstPtr&);
    //void updateDart(const QMqttMessage &msg);

public slots:

    void initGame();
    void updatePlayers();
    void setGameActive(bool active);


private slots:
    void nameChanged(int index);
    void playerNumChanged(int index);

    void toggleSound(bool off);
    void togglePaintDarts(bool on);
    void managePlayers();

 //   void listMenuRequested(QPoint p);

    void openStatViewer();
    void createNewGame(bool show_diag=true);

signals:
    void signalPlayerChange(int index, QSharedPointer<Player> player);
    void signalPlayerNumChange(int number, QList<QSharedPointer<Player>> dbPlayerOrder);
    void newCvDart(QPointF loc, QPointF intendedLoc);

private:
   /* ros::NodeHandle n;
    ros::Subscriber sub;
    QSharedPointer<ros::AsyncSpinner> spinner;
*/
    QList<QSharedPointer<Player>> dbPlayerOrder;
    QMap<QString,QSharedPointer<Player>> db_players;

    bool saveDialogYes(QSharedPointer<Player> winner);
    void keyPressEvent(QKeyEvent* event);

    void initUi();

    QSharedPointer<StatViewer> statViewer;

    QFont scoreFont;
    QPalette p, pn, highlightScore;
    QSignalMapper* playerNameMapper;
    QSignalMapper* playerNumMapper;

    QList<QFrame*> playerFrames;


    QList<PlayerWidget*> playerWidgets;
    QLabel* gameStatusLabel;
    QPushButton* toggleSoundsButton;
    QPushButton* togglePaintDartsButton;

    Ui::MainWindow *ui;
    DBManager* database;
    GameManager* gameManager;

    void fakeResize();

    bool blocked;

};

#endif // MAINWINDOW_H
