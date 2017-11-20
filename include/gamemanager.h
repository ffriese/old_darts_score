#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QPainterPath>
#include <QMap>
#include "dbmanager.h"
#include "game.h"
#include "player.h"
#include "dartboard.h"
#include "dart.h"
#include <QSharedPointer>

class GameManager: public QObject
{
    Q_OBJECT
public:
    GameManager();


    void newGame(QString game_name, QList<Game::VARIANT> variants = {});

    bool isActive();

    bool inProgress(){
        return game->getStatus() == Game::IN_PROGRESS;
    }

    int getSetsToWin(){
        return game->getSetsToWin();
    }
    int getLegsToWin(){
        return game->getLegsToWin();
    }

    int getWinner(){
        return game->getWinner();
    }

    int getLegWinner(){
        return game->getLegWinner();
    }

    QSharedPointer<Game> getGame(){
        return game;
    }

    QMap<QString,QList<Game::VARIANT>> getOptionGroups(){
        return game->getOptionGroups();
    }

    QString getVariantString(Game::VARIANT var){
        return game->getVariantString(var);
    }

    QString getStatusString(){
        QString name = game->getGameName();
        QString vars = game->getVariantStrings();
        if(name == "X01"){
            name = QString::number(game->getStartScore());
            vars = vars.replace(", "+name,"");
            vars = vars.replace(name+", ","");
        }
        return name + " [" + vars +"]";
    }

    bool isDefaultVariant(Game::VARIANT var){
        return game->isDefaultVariant(var);
    }

    QList<QSharedPointer<Player>> getPlayers(){
        return players;
    }

    QSharedPointer<Player> getCurrentLegBeginner(){
        return game->getCurrentLegBeginner();
    }

    QSharedPointer<Player> getCurrentSetBeginner(){
        return game->getCurrentSetBeginner();
    }

    QSharedPointer<Player> getCurrentPlayer(){
        return game->getCurrentPlayer();
    }

    QSharedPointer<Player> getLastPlayer(){
        return game->getLastPlayer();
    }

    void continueGame();

private:


    QSharedPointer<Game> game;

   // int playerTurn;
    int legID;
    DartBoard* dartBoard;
    DBManager* db;
   // bool gameActive;

    QList<QSharedPointer<Player>> players;

    //HANDLED BY GAME
  //  QPair<int,DartBoard::BED> generateTarget();
  //  void handleDart(Dart* dart);
  //  void checkTurn();


public slots:

  //  void newGame();
    void setGame(QSharedPointer<Game> _game);

    void resetGame();
// DEFINITELY STAYS HERE
    void handleUserDart(QPointF loc, QPointF intendedLoc=QPointF());
    void handleAIDartRequest(QPointF intendedLoc);
    void throwAIDart();
    void addBounceOut();

    void changeNumOfPlayers(int number, QList<QSharedPointer<Player>> dbPlayerOrder);
    void requestPlayerChange(int index, QSharedPointer<Player> player);
//
    //check later
    void undoLastDart();

signals:

    void signalGameStarted();
    void signalGameActive(bool active);

    void clearTakeRequest();

    void signalPlayerUpdate();
    void updateTurn(int playerIndex);

};

#endif // GAMEMANAGER_H
