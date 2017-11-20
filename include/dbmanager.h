#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <qsqldatabase.h>
#include "player.h"
#include "game.h"

class DBManager
{
public:

    static DBManager* getInstance(){
        return getInstance("db/test.db");
    }

    static DBManager* getInstance(QString _path){
            static DBManager* instance;
            static QString path;
            if(instance==NULL){
                path=_path;
                instance = new DBManager(path);
            }else if(_path!=path){
                path=_path;
                DBManager* tmp = new DBManager(path);
                delete instance;
                instance = tmp;
            }
            return instance;
    }

    bool addPlayer(const QString& name, double hDev = 40.0, double vDev = 40.0, bool isAi = false);
    QList<QSharedPointer<Player>> getPlayers(bool update = false);
    QSharedPointer<Game> getGame(int id);

    int getNextGameID();
    int getNextLegID();

    bool gameFinished(int id);

    bool gameExists(int id);
    bool legExists(int id);


    bool saveGame(QSharedPointer<Game> game, QString& err);
    bool removeGameFromDatabase(int id);

    double getAverage(QSharedPointer<Player> player, QDate start=QDate(), QDate end=QDate());
    double getHDev(QSharedPointer<Player> player, QDate start=QDate(), QDate end=QDate());
    double getVDev(QSharedPointer<Player> player, QDate start=QDate(), QDate end=QDate());
    double getHStdDev(QSharedPointer<Player> player, QDate start=QDate(), QDate end=QDate());
    double getVStdDev(QSharedPointer<Player> player, QDate start=QDate(), QDate end=QDate());
    void updatePlayerStats(QSharedPointer<Player> player);

    QDate getEarliestDate();
private:

    DBManager(QString path);
    QSqlDatabase m_db;


    QList<QSharedPointer<Player>> databasePlayers;

    int getCurrentId(QString table);
    int getNextId(QString table);
    bool checkForEntry(QString table, QString field, int id);
};

#endif // DBMANAGER_H
