#include "dbmanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QApplication>
#include <QDir>
#include "gamefactory.h"
#include <math.h>

DBManager::DBManager(QString path)
{

    databasePlayers.clear();
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);


    if(!QFile::exists(path)){

        if (!m_db.open())
        {
            QString newDir = path.left(path.lastIndexOf("/"));
            QDir dir;
            if(!dir.mkpath(newDir)){
                qDebug()<<"ERROR: path "+path+" could not be created!";
                return;
            }
        }


        if (!m_db.open())
        {
            qDebug() << "ERROR: creating database failed ("+ path + ")";
        }
        else
        {

            QSqlQuery query("PRAGMA foreign_keys = ON",m_db);
            if(!query.exec()){
                qDebug()<<"ERROR: enabling foreign keys failed!";
            }else{
                qDebug()<<"successfully enabled foreign keys";
            }


            QList<QString> queries;

            queries << "CREATE TABLE Players ("
                           "`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                           "`Name`	TEXT NOT NULL UNIQUE,"
                           "`HDev`	REAL NOT NULL DEFAULT 35.0 CHECK(HDev >= 0),"
                           "`VDev`	REAL NOT NULL DEFAULT 45.0 CHECK(VDev >= 0),"
                           "`isAI`	INTEGER NOT NULL DEFAULT 0 CHECK(isAI > - 1 AND isAI < 2),"
                           "`numTakes`	INTEGER"
                           ")";

            queries << "CREATE TABLE Games ("
                            "`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                            "`Name`	TEXT NOT NULL,"
                            "`Variants`	TEXT,"
                            "`SetsToWin`	INTEGER,"
                            "`LegsToWin`	INTEGER,"
                            "`Finished`	INTEGER NOT NULL CHECK(Finished>=0 AND Finished<2)"
                            ")";

            queries << "CREATE TABLE Sets ("
                           "`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                           "`GameID`	INTEGER NOT NULL,"
                           "`WinnerID`	INTEGER NOT NULL,"
                           "`BeginnerID`	INTEGER NOT NULL,"
                           "FOREIGN KEY(`GameID`) REFERENCES Games (`ID`) ON DELETE CASCADE,"
                           "FOREIGN KEY(`WinnerID`) REFERENCES Players (`ID`) ON DELETE SET NULL,"
                           "FOREIGN KEY(`BeginnerID`) REFERENCES Players (`ID`)  ON DELETE SET NULL"
                           ")";

            queries << "CREATE TABLE Legs ("
                           "`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                           "`SetID`	INTEGER NOT NULL,"
                           "`WinnerID`	INTEGER NOT NULL,"
                           "`BeginnerID`	INTEGER NOT NULL,"
                           "FOREIGN KEY(`SetID`) REFERENCES Sets (`ID`) ON DELETE CASCADE,"
                           "FOREIGN KEY(`WinnerID`) REFERENCES Players (`ID`) ON DELETE SET NULL,"
                           "FOREIGN KEY(`BeginnerID`) REFERENCES Players(`ID`)  ON DELETE SET NULL"
                           ")";

            queries << "CREATE TABLE Takes ("
                           "`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                           "`LegID`	INTEGER NOT NULL,"
                           "`PlayerID`	INTEGER,"
                           "`Dart1ID`	INTEGER,"
                           "`Dart2ID`	INTEGER,"
                           "`Dart3ID`	INTEGER,"
                           "`RESULT`	TEXT NOT NULL DEFAULT 'NONE',"
                           "FOREIGN KEY(`LegID`) REFERENCES Legs (`ID`) ON DELETE CASCADE,"
                           "FOREIGN KEY(`PlayerID`) REFERENCES Players (`ID`) ON DELETE SET NULL,"
                           "FOREIGN KEY(`Dart1ID`) REFERENCES Darts (`ID`) ON DELETE CASCADE,"
                           "FOREIGN KEY(`Dart2ID`) REFERENCES Darts (`ID`) ON DELETE SET NULL,"
                           "FOREIGN KEY(`Dart3ID`) REFERENCES Darts (`ID`) ON DELETE SET NULL"
                           ")";

            queries << "CREATE TABLE Darts ("
                            "`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                            "`Score`	INTEGER NOT NULL,"
                            "`HitLocation`	TEXT NOT NULL,"
                            "`GoalLocation`	TEXT,"
                            "`HDev`	REAL,"
                            "`VDev`	REAL,"
                            "`Intent`	TEXT DEFAULT 'SCORE',"
                            "`Date`	TEXT,"
                            "`Generated`	INTEGER NOT NULL DEFAULT 0 CHECK(Generated > - 1 AND Generated < 2)"
                            ")";

            queries << "CREATE TRIGGER delete_darts BEFORE DELETE ON Takes "
                       "BEGIN "
                       "  DELETE FROM Darts WHERE ID=OLD.Dart1ID OR ID=OLD.Dart2ID OR ID=OLD.Dart3ID;"
                       "END";


            //ADD AI-PLAYERS!
            queries << "INSERT INTO Players (Name,HDev,VDev,isAI) VALUES ('AI - Easy',         29,   35,  1)";
            queries << "INSERT INTO Players (Name,HDev,VDev,isAI) VALUES ('AI - Medium',       20,   28,  1)";
            queries << "INSERT INTO Players (Name,HDev,VDev,isAI) VALUES ('AI - Hard',         13,   18,  1)";
            queries << "INSERT INTO Players (Name,HDev,VDev,isAI) VALUES ('AI - Very Hard',    10,   15,  1)";
            queries << "INSERT INTO Players (Name,HDev,VDev,isAI) VALUES ('AI - World Class',  4.5,  9.0, 1)";



            for(QString query: queries){
                QSqlQuery q = m_db.exec(query);
                if(q.lastError().text().trimmed()!=""){
                    qDebug()<<"Database creation error:"<<q.lastError().text();
                }
            }
            qDebug()<<"successfully created new database at "<<path;

        }
    }else{
        if (!m_db.open())
        {
            qDebug() << "ERROR: connection to database failed";
        }
        else
        {
            qDebug() << "successfully connected to database.";
            QSqlQuery query("PRAGMA foreign_keys = ON",m_db);

         //   qDebug()<<"prepared statement support:"<<query.driver()->hasFeature(QSqlDriver::PreparedQueries);
         //   qDebug()<<"pos placeholder support:"<<query.driver()->hasFeature(QSqlDriver::PositionalPlaceholders);
         //   qDebug()<<"name placeholder support:"<<query.driver()->hasFeature(QSqlDriver::NamedPlaceholders);

            if(!query.exec()){
                qDebug()<<"ERROR: enabling foreign keys failed!";
            }else{
                qDebug()<<"successfully enabled foreign keys";
            }
        }
    }
}

bool DBManager::addPlayer(const QString& name, double hDev, double vDev, bool isAi)
{
   bool success = false;
   // you should check if args are ok first...
   QSqlQuery query;

   query.prepare("INSERT INTO Players (Name,HDev,VDev,isAI) VALUES (:name,:hdev,:vdev,:isai)");
   query.bindValue(":name", name);
   query.bindValue(":hdev", hDev);
   query.bindValue(":vdev", vDev);
   query.bindValue(":isai", isAi);
   if(query.exec())
   {
       success = true;
       qDebug()<<"successfully added player"<<name;
   }
   else
   {
        qDebug() << "ERROR: adding player failed:  "
                 << query.lastError();
   }

   return success;
}

QList<QSharedPointer<Player>> DBManager::getPlayers(bool update){

    if(!databasePlayers.isEmpty() && !update){
        return databasePlayers;
    }

    databasePlayers.clear();
    QSqlQuery query("SELECT * FROM players ORDER BY isAi , numTakes DESC");
    if(query.exec())
    {
        int idID = query.record().indexOf("id");
        int idName = query.record().indexOf("name");
        int idHDev = query.record().indexOf("hdev");
        int idVDev = query.record().indexOf("vdev");
        int idIsAI = query.record().indexOf("isai");
        while (query.next())
        {
           int id = query.value(idID).toInt();
           QString name = query.value(idName).toString();
           double hDev = query.value(idHDev).toDouble();
           double vDev = query.value(idVDev).toDouble();
           bool isAI = query.value(idIsAI).toBool();
           databasePlayers.append(QSharedPointer<Player>(new Player(id,name,isAI,hDev,vDev)));
        }
    }
    else
    {
         qDebug() << "ERROR: getting players failed: "
                  << query.lastError();
    }
    return databasePlayers;
}

QSharedPointer<Game> DBManager::getGame(int id){
   if(!this->gameExists(id)){
        qDebug()<<"ERROR: game"<<id<<"does not exist!";
        return QSharedPointer<Game>();
    }
    QSqlQuery query;
    query.prepare("SELECT * FROM Games WHERE ID=:id");
    query.bindValue(":id",id);


    if(query.exec()){
        int idGame = query.record().indexOf("Name");
        int idLtw = query.record().indexOf("LegsToWin");
        int idVariants = query.record().indexOf("Variants");
        if(query.next()){
            QString gamename = query.value(idGame).toString();
            int legsToWin = query.value(idLtw).toInt();
            QList<Game::VARIANT> variants;
            QList<QString> vars= query.value(idVariants).toString().split(",");
            for(QString var : vars){
                variants.append(static_cast<Game::VARIANT>(var.toInt()));
            }


            QSharedPointer<Game> game = GameFactory::getInstance()->createGame(gamename,legsToWin,variants,id);
            qDebug()<<"got game!";
            return game;

        }else{
            qDebug() << "ERROR: getting game failed: "
                     << query.lastError();
            qDebug() << query.executedQuery();
        }

    }
    else
    {
         qDebug() << "ERROR: getting game failed: "
                  << query.lastError();
    }
    return QSharedPointer<Game>();

}

int DBManager::getNextId(QString table){
    return getCurrentId(table)+1;
}

int DBManager::getCurrentId(QString table){
    QSqlQuery query;
    query.prepare("SELECT * FROM SQLITE_SEQUENCE WHERE name=:table");
    query.bindValue(0, table);
    if(query.exec()){
        int seq= query.record().indexOf("seq");
        query.next();
        return (query.record().value(seq).toInt());
    }
    else
    {
         qDebug() << "ERROR: request failed: "
                  << query.lastError();
         return -2;
    }
}

bool DBManager::checkForEntry(QString table, QString field, int id){
    QSqlQuery query;
    bool prep =query.prepare("SELECT EXISTS ( SELECT 1 FROM "+table+" WHERE "+field+" = :id LIMIT 1 )");
    if(!prep){
        qDebug() << "ERROR: existance request failed: "
                 << query.lastError() <<query.lastQuery();
        return false;
    }
    query.bindValue(":id",id);

    if(query.exec()){
        query.next();
        return query.record().value(0).toBool();
    }
    else
    {
         qDebug() << "ERROR: existance request failed: "
                  << query.lastError() <<query.lastQuery();
    }
    return true;
}


int DBManager::getNextGameID(){
    return getNextId("Games");
}


int DBManager::getNextLegID(){
    return getNextId("Legs");
}


bool DBManager::gameExists(int id){
    return checkForEntry("Games","ID",id);
}

bool DBManager::gameFinished(int id){
    QSqlQuery query;
    query.prepare("SELECT * FROM Games WHERE ID=:id");
    query.bindValue(":id", id);
    if(query.exec()){
        int seq= query.record().indexOf("Finished");
        query.next();
        return (query.record().value(seq).toBool());
    }
    else
    {
         qDebug() << "ERROR: request failed: "
                  << query.lastError();
         return false;
    }
}


bool DBManager::removeGameFromDatabase(int id){
    bool success = false;




    return success;
}

bool DBManager::saveGame(QSharedPointer<Game> game, QString& err){
    bool success = false;
    // you should check if args are ok first...

    // GET GAME INFORMATION
    QString name = game->getGameName();
    QString variants = "";
    QList<Game::VARIANT> vars = game->getVariants();
    foreach (Game::VARIANT v, vars){
        variants += QString::number((int)v) + ",";
    }
    if(variants.length()>0)
        variants.chop(1);
    int gameID = game->getID();
    int legsToWin = game->getLegsToWin();
    int setsToWin = game->getSetsToWin();
    bool finnished = game->getStatus() == Game::FINNISHED;
    qDebug()<<name<<variants<<legsToWin<<setsToWin<<finnished;


    m_db.transaction();

    // SAVE GAME TO DB
    QSqlQuery gamequery,setquery,legquery,dartquery,takequery;
    gamequery.prepare("INSERT INTO Games (Name,Variants,LegsToWin,SetsToWin,Finished) VALUES (:name,:vars,:ltw,:stw,:fin)");
    setquery.prepare("INSERT INTO Sets (GameID,WinnerID,BeginnerID) VALUES (:gameid,:winnerid,:beginnerid)");
    legquery.prepare("INSERT INTO Legs (SetID,WinnerID,BeginnerID) VALUES (:setid,:winnerid,:beginnerid)");
    dartquery.prepare("INSERT INTO Darts (Score,HitLocation,GoalLocation, HDev, VDev, Intent, Date, Generated) VALUES (:score,:hloc,:gloc,:hdev,:vdev,:intent, :date, :gen)");
    takequery.prepare("INSERT INTO Takes (LegID,PlayerID,Dart1ID,Dart2ID,Dart3ID,RESULT) VALUES (:legid,:playerid,:dart1id,:dart2id,:dart3id,:result)");
    //qDebug()<<gamequery.driver()->hasFeature(QSqlDriver::LastInsertId);
    gamequery.bindValue(":name", name);
    gamequery.bindValue(":vars", variants);
    gamequery.bindValue(":ltw", legsToWin);
    gamequery.bindValue(":stw", setsToWin);
    gamequery.bindValue(":fin", finnished);
    if(gamequery.exec())
    {
        //gameID = query.driver()->LastInsertId;
        gameID = getCurrentId("Games");
       // qDebug()<<"successfully added game"<<name<<gameID;
    }
    else
    {
         err =  "ERROR: adding game failed:  " + gamequery.lastError().text();
         m_db.rollback();
         err = err + "\nrolled back";
         qDebug()<<err;
         return false;
    }

    // GET SET INFORMATION
    QList<QSharedPointer<Set> > sets = game->getSets();
    foreach(QSharedPointer<Set> set, sets){
        int setID;
        int winnerID = set->getWinner()->getID();
        int beginnerID = set->getBeginner()->getID();
        // SAVE SET TO DB
        setquery.bindValue(":gameid", gameID);
        setquery.bindValue(":winnerid", winnerID);
        setquery.bindValue(":beginnerid", beginnerID);
        if(setquery.exec())
        {
            //setID=query.driver()->LastInsertId;
            setID=getCurrentId("Sets");
           // qDebug()<<"successfully added set"<<setID;
        }
        else
        {
             err = "ERROR: adding set failed:  " + setquery.lastError().text();
             m_db.rollback();
             err = err + "\nrolled back!";
             qDebug()<<err;
             return false;
        }

        // GET LEG INFORMATION
        QList<QSharedPointer<Leg> > legs = set->getLegs();
        foreach(QSharedPointer<Leg> leg, legs){
            int legID;
            int winnerID = leg->getWinner()->getID();
            int beginnerID = leg->getBeginner()->getID();
            // SAVE LEG TO DB
            legquery.bindValue(":setid", setID);
            legquery.bindValue(":winnerid", winnerID);
            legquery.bindValue(":beginnerid", beginnerID);
            if(legquery.exec())
            {
                //legID=query.driver()->LastInsertId;
                legID=getCurrentId("Legs");
               // qDebug()<<"successfully added leg"<<legID;
            }
            else
            {
                 err = "ERROR: adding leg failed:  " + legquery.lastError().text();
                 m_db.rollback();
                 err = err + "\nrolled back!";
                 qDebug()<<err;
                 return false;
            }
            // GET TAKE INFORMATION
            QList<QSharedPointer<Take> > takes = leg->getTakes();
            foreach(QSharedPointer<Take> take, takes){
                int takeID;
                QSharedPointer<Player> player = game->findTakeOwner(take);
                int playerID = !player.isNull() ? player->getID() : -1;
                if(playerID==-1){
                    qDebug()<<"big phat ERROR!!!";
                    qDebug()<<game->getSets().indexOf(set)<<set->getLegs().indexOf(leg);
                    foreach(QSharedPointer<Dart> dart, take->getDarts()){
                        qDebug()<< dart->getScore();
                        qDebug()<< QString::number(dart->getExactLocation().x()) + "," + QString::number(dart->getExactLocation().y());
                        qDebug()<< QString::number(dart->getIntendedLoctaion().x()) + "," + QString::number(dart->getIntendedLoctaion().y());
                        qDebug()<< dart->getHDev();
                        qDebug()<< dart->getVDev();
                        qDebug()<< "SCORE"; // NOT YET IMPLEMENTED
                        qDebug()<< dart->getDateString();
                        qDebug()<< dart->isGenerated();
                        qDebug()<< take->getResult();
                    }
                    m_db.rollback();
                    return false;
                }
                int result = (int)take->getResult();
                QList<int> dartIDs;
                // GET DART INFORMATION
                QList<QSharedPointer<Dart> > darts = take->getDarts();
                foreach(QSharedPointer<Dart> dart, darts){
                    int dartID;
                    int score = dart->getScore();
                    QString hitLocation = QString::number(dart->getExactLocation().x()) + "," + QString::number(dart->getExactLocation().y());
                    QString goalLocation = QString::number(dart->getIntendedLoctaion().x()) + "," + QString::number(dart->getIntendedLoctaion().y());
                    double hDev = dart->getHDev();
                    double vDev = dart->getVDev();
                    QString intent = "SCORE"; // NOT YET IMPLEMENTED
                    QString date = dart->getDateString();
                    bool generated = dart->isGenerated();
                    //SAVE DART TO DB
                    dartquery.bindValue(":score", score);
                    dartquery.bindValue(":hloc", hitLocation);
                    dartquery.bindValue(":gloc", goalLocation);
                    dartquery.bindValue(":hdev", hDev);
                    dartquery.bindValue(":vdev", vDev);
                    dartquery.bindValue(":intent", intent);
                    dartquery.bindValue(":date", date);
                    dartquery.bindValue(":gen", generated);
                    if(dartquery.exec())
                    {
                        //dartID=query.driver()->LastInsertId;
                        dartID=getCurrentId("Darts");
                       // qDebug()<<"successfully added dart"<<dartID;
                    }
                    else
                    {
                        qDebug()<<dartquery.boundValues();
                        err = "ERROR: adding dart failed:  " + dartquery.lastError().text();
                        m_db.rollback();
                        err = err + "\nrolled back!";
                        qDebug()<<err;
                         return false;
                    }
                    dartIDs.append(dartID);
                // SAVE TAKE TO DB
                }
                takequery.bindValue(":legid", legID);
                takequery.bindValue(":playerid", playerID);
                takequery.bindValue(":dart1id", dartIDs.count()>0 ? dartIDs[0] : QVariant());
                takequery.bindValue(":dart2id", dartIDs.count()>1 ? dartIDs[1] : QVariant());
                takequery.bindValue(":dart3id", dartIDs.count()>2 ? dartIDs[2] : QVariant());
                takequery.bindValue(":result", result);
                if(takequery.exec())
                {
                    //takeID=query.driver()->LastInsertId;
                    takeID=getCurrentId("Takes");
                   // qDebug()<<"successfully added take"<<takeID;
                }
                else
                {
                    qDebug()<<dartquery.boundValues();
                    qDebug()<<takequery.boundValues();
                    err = "ERROR: adding take failed:  " + takequery.lastError().text();
                    m_db.rollback();
                    err = err + "\nrolled back!";
                    qDebug()<<err;
                     return false;
                }
            }
        }
    }

    success = m_db.commit();

    //UPDATE PLAYER TAKECOUNT, PLAYER DEV
   // m_db.transaction();
    foreach(QSharedPointer<Player> player,game->getPlayers()){
        if(player->isAi()){
            continue;
        }

        updatePlayerStats(player);
    }
   // m_db.commit();


    return success;
}

QDate DBManager::getEarliestDate(){

    QString dtstr="";

    QSqlQuery query("SELECT MIN(DATE) AS MDATE FROM (SELECT DATE FROM DARTS)");

    if(query.exec()){
        int md= query.record().indexOf("MDate");
        query.next();
        dtstr = query.record().value(md).toString();
    }else{
        qDebug()<<"getEarliestDate failed";
        qDebug()<<query.lastError();
        return QDate::currentDate();
    }
    if(dtstr.isEmpty()){
        return QDate::currentDate();
    }
    return QDate::fromString(dtstr,"yyyy-MM-dd");
}

void DBManager::updatePlayerStats(QSharedPointer<Player> player){
    QString id = QString::number(player->getID());
    //TAKECOUNT
    QString updatequery = QString("UPDATE Players SET numTakes = (SELECT Count(ID) FROM Takes WHERE PlayerID = ")+
            id+QString(") WHERE ID = ")+id;
    QSqlQuery query(updatequery.toStdString().c_str());
    if(!query.exec()){
        qDebug()<<"update takenumbers failed";
        qDebug()<<query.lastError();
    }

    //HDEV
    updatequery = QString("UPDATE Players SET HDev = (SELECT AVG(HDev) FROM ("
    "SELECT HDev FROM Darts WHERE Generated is not '1' AND GoalLocation is not '0,0' AND ID IN ("
    "SELECT Dart1ID FROM Takes WHERE Dart1ID is not null AND PlayerID = ")+ id+QString(" UNION "
    "SELECT Dart2ID FROM Takes WHERE Dart2ID is not null AND PlayerID = ")+ id+QString(" UNION "
    "SELECT Dart3ID FROM Takes WHERE Dart3ID is not null AND PlayerID = ")+ id+QString("))) WHERE ID = ")+ id;

    QSqlQuery query2(updatequery.toStdString().c_str());
    if(!query2.exec()){
        qDebug()<<"update hdev failed";
        qDebug()<<updatequery;
        qDebug()<<query2.lastError();
    }
    //VDEV
    updatequery = QString("UPDATE Players SET VDev = (SELECT AVG(VDev) FROM ("
    "SELECT VDev FROM Darts WHERE Generated is not '1' AND GoalLocation is not '0,0' AND ID IN ("
    "SELECT Dart1ID FROM Takes WHERE Dart1ID is not null AND PlayerID = ")+ id+QString(" UNION "
    "SELECT Dart2ID FROM Takes WHERE Dart2ID is not null AND PlayerID = ")+ id+QString(" UNION "
    "SELECT Dart3ID FROM Takes WHERE Dart3ID is not null AND PlayerID = ")+ id+QString("))) WHERE ID = ")+ id;

    QSqlQuery query3(updatequery.toStdString().c_str());
    if(!query3.exec()){
        qDebug()<<"update vdev failed";
        qDebug()<<updatequery;
        qDebug()<<query3.lastError();
    }
    updatequery= QString("SELECT HDev,VDev FROM Players WHERE ID = ")+id;
    QSqlQuery query4(updatequery);
    if(query4.exec()){
        int hd= query4.record().indexOf("HDev");
        int vd= query4.record().indexOf("VDev");
        query4.next();
        double hdev = query4.record().value(hd).toDouble();
        double vdev = query4.record().value(vd).toDouble();
        player->setHDev(hdev);
        player->setVDev(vdev);
    }else{
        qDebug()<<"getting new playerstats failed";
        qDebug()<<updatequery;
        qDebug()<<query3.lastError();
    }
}


double DBManager::getAverage(QSharedPointer<Player> player, QDate start, QDate end){
    int id=player->getID();
    QString gen = player->isAi()? "is" : "is not";
    QString from = start.isNull()? "" : " AND DATE >= '"+start.toString("yyyy-MM-dd")+"'";
    QString to = end.isNull()? "" : " AND DATE <= '"+end.toString("yyyy-MM-dd")+"'";
    QString avgQuery = QString("SELECT AVG(Score)*3 AS avgScore FROM "
                          "(SELECT Score FROM Darts WHERE Generated %1 '1' %2%3 AND ID IN "
                          "(SELECT Dart1ID FROM Takes WHERE Dart1ID is not null AND PlayerID = '%4' UNION"
                          " SELECT Dart2ID FROM Takes WHERE Dart2ID is not null AND PlayerID = '%4' UNION"
                          " SELECT Dart3ID FROM Takes WHERE Dart3ID is not null AND PlayerID = '%4'))")
            .arg(gen).arg(from).arg(to).arg(id);


    QSqlQuery query(avgQuery.toStdString().c_str());
    if(query.exec()){
        int avg= query.record().indexOf("avgScore");
        query.next();
        return (query.record().value(avg).toDouble());
    }else{
        qDebug()<<"getAv failed";
        qDebug()<<query.lastError();
        return -1;
    }
}

double DBManager::getHDev(QSharedPointer<Player> player, QDate start, QDate end){
    int id=player->getID();
    QString from = start.isNull()? "" : " AND DATE >= '"+start.toString("yyyy-MM-dd")+"'";
    QString to = end.isNull()? "" : " AND DATE <= '"+end.toString("yyyy-MM-dd")+"'";

    QString hDevQuery = QString("SELECT AVG(HDev) AS HDev FROM "
                          "(SELECT HDev FROM Darts WHERE Generated is not '1' %2%3 AND GoalLocation is not '0,0' AND ID IN "
                          "(SELECT Dart1ID FROM Takes WHERE Dart1ID is not null AND PlayerID = '%4' UNION"
                          " SELECT Dart2ID FROM Takes WHERE Dart2ID is not null AND PlayerID = '%4' UNION"
                          " SELECT Dart3ID FROM Takes WHERE Dart3ID is not null AND PlayerID = '%4'))")
            .arg(from).arg(to).arg(id);


    QSqlQuery query(hDevQuery.toStdString().c_str());

    if(query.exec()){
        int hdev= query.record().indexOf("HDev");
        query.next();
        return (query.record().value(hdev).toDouble());
    }else{
        qDebug()<<"getHDev failed";
        qDebug()<<query.lastError();
        return -1;
    }
}

double DBManager::getHStdDev(QSharedPointer<Player> player, QDate start, QDate end){
    int id=player->getID();
    QString from = start.isNull()? "" : " AND DATE >= '"+start.toString("yyyy-MM-dd")+"'";
    QString to = end.isNull()? "" : " AND DATE <= '"+end.toString("yyyy-MM-dd")+"'";

    QString hDevQuery=QString("SELECT Sum(HDev*HDev)/(Count(HDev)-1) AS HStdDev FROM Darts "
                              "WHERE Generated is not '1' AND GoalLocation is not '0,0'")+
            from+to+
            QString(" AND ID IN "
            "(SELECT Dart1ID FROM Takes WHERE Dart1ID is not null AND PlayerID = '")+QString::number(id)+
            QString("' UNION SELECT Dart2ID FROM Takes WHERE Dart2ID is not null AND PlayerID = '")+QString::number(id)+
            QString("' UNION SELECT Dart3ID FROM Takes WHERE Dart3ID is not null AND PlayerID = '")+QString::number(id)+
            QString("')");

    QSqlQuery query(hDevQuery.toStdString().c_str());

    if(query.exec()){
        int hdev= query.record().indexOf("HStdDev");
        query.next();
        return sqrt(query.record().value(hdev).toDouble());
    }else{
        qDebug()<<"getHStdDev failed";
        qDebug()<<query.lastError();
        return -1;
    }
}

double DBManager::getVDev(QSharedPointer<Player> player, QDate start, QDate end){
    int id=player->getID();
    QString from = start.isNull()? "" : " AND DATE >= '"+start.toString("yyyy-MM-dd")+"'";
    QString to = end.isNull()? "" : " AND DATE <= '"+end.toString("yyyy-MM-dd")+"'";
    QString vDevQuery = QString("SELECT AVG(VDev) AS VDev FROM "
                          "(SELECT VDev FROM Darts WHERE Generated is not '1' %1%2 AND GoalLocation is not '0,0' AND ID IN "
                          "(SELECT Dart1ID FROM Takes WHERE Dart1ID is not null AND PlayerID = '%3' UNION"
                          " SELECT Dart2ID FROM Takes WHERE Dart2ID is not null AND PlayerID = '%3' UNION"
                          " SELECT Dart3ID FROM Takes WHERE Dart3ID is not null AND PlayerID = '%3'))")
            .arg(from).arg(to).arg(id);


    QSqlQuery query(vDevQuery.toStdString().c_str());

    if(query.exec()){
        int vdev= query.record().indexOf("VDev");
        query.next();
        return (query.record().value(vdev).toDouble());
    }else{
        qDebug()<<"getvDev failed";
        qDebug()<<query.lastError();
        return -1;
    }
}

double DBManager::getVStdDev(QSharedPointer<Player> player, QDate start, QDate end){
    int id=player->getID();
    QString from = start.isNull()? "" : " AND DATE >= '"+start.toString("yyyy-MM-dd")+"'";
    QString to = end.isNull()? "" : " AND DATE <= '"+end.toString("yyyy-MM-dd")+"'";
    QString vDevQuery=QString("SELECT Sum(VDev*VDev)/(Count(VDev)-1) AS VStdDev FROM Darts "
                              "WHERE Generated is not '1' AND GoalLocation is not '0,0'")+
            from+to+
            QString(" AND ID IN "
            "(SELECT Dart1ID FROM Takes WHERE Dart1ID is not null AND PlayerID = '")+QString::number(id)+
            QString("' UNION SELECT Dart2ID FROM Takes WHERE Dart2ID is not null AND PlayerID = '")+QString::number(id)+
            QString("' UNION SELECT Dart3ID FROM Takes WHERE Dart3ID is not null AND PlayerID = '")+QString::number(id)+
            QString("')");

    QSqlQuery query(vDevQuery.toStdString().c_str());

    if(query.exec()){
        int vdev= query.record().indexOf("VStdDev");
        query.next();
        return sqrt((query.record().value(vdev).toDouble()));
    }else{
        qDebug()<<"getvStdDev failed";
        qDebug()<<query.lastError();
        return -1;
    }
}

bool DBManager::legExists(int id){
    return checkForEntry("Legs","ID",id);
}

