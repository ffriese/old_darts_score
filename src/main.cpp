#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QSharedPointer>
#include <MQTTClient.h>
#include <MQTTAsync.h>
#include <MQTTClientPersistence.h>

#include <QDebug>
//#include <ros/ros.h>




int main(int argc, char *argv[])
{

    //ros::init(argc, argv, "darts_listener");
    QApplication a(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    MainWindow w;
    w.show();

    return a.exec();
}
