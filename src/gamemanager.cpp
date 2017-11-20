#include "gamemanager.h"
#include "utils.h"
#include "gamefactory.h"
#include <QRegularExpression>
#include <QDebug>
#include <QVector2D>
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>

GameManager::GameManager()
{
    dartBoard = DartBoard::getInstance();
    db = DBManager::getInstance();
    game = QSharedPointer<Game>();

    QList<QSharedPointer<Player>> dbplayers = db->getPlayers();

    int numOfStartPlayers = 2;

    for(int i = 0; i<(std::min(dbplayers.count(),numOfStartPlayers)); i++){
        QSharedPointer<Player> p = dbplayers.at(i);
        players.append(p);
    }
   // qDebug()<<"next gameid:"<<db->getNextGameID();
}

bool GameManager::isActive(){
    return game->getStatus() == Game::IN_PROGRESS || game->getStatus() == Game::LEG_FINNISHED;
}

void GameManager::newGame(QString game_name, QList<Game::VARIANT> variants){
    setGame(GameFactory::getInstance()->createGame(game_name,3,variants,-1,2));
}

void GameManager::setGame(QSharedPointer<Game> _game){
    game = _game;
    qDebug()<<"created Game:"<<game->getID()<<game->getGameName()<<game->getVariantStrings();
    game->reset();
    qDebug()<<"game status:"<<game->getStatus()<<isActive();

    for(QSharedPointer<Player> player:players){
        player->reset(game->getStartScore());
    }

    emit signalPlayerUpdate();
    signalGameActive(isActive());
}

void GameManager::resetGame(){

    // ToDO: GET HANDICAPS
    QMap<QSharedPointer<Player>,QList<Game::HANDICAP>> handicaps = QMap<QSharedPointer<Player>,QList<Game::HANDICAP>>();
    game->start(players, handicaps);
    game->reset();


    emit signalGameStarted();

    emit updateTurn(game->getPlayerTurn());
    emit signalGameActive(isActive());
    emit clearTakeRequest();

    emit signalPlayerUpdate();
}

void GameManager::continueGame(){
    qDebug()<<game->getStatus();
    switch (game->getStatus()) {

    case Game::LEG_FINNISHED:
        game->continueGame();
        emit updateTurn(game->getPlayerTurn());
        emit clearTakeRequest();
        emit signalPlayerUpdate();
        break;
    case Game::FINNISHED:
        qDebug()<<"save";
    case Game::WAITING_FOR_USER_INPUT:
        resetGame();
        break;
    case Game::IN_PROGRESS:
        break;
    default:
        break;
    }

    emit updateTurn(game->getPlayerTurn());
    emit clearTakeRequest();
    emit signalPlayerUpdate();
}


void GameManager::requestPlayerChange(int index, QSharedPointer<Player> player){
    players.at(index)->change(player->getID(),player->getName(),player->isAi(),player->getHDev(),player->getVDev());

    for(QSharedPointer<Player> player:players){
        player->reset(game->getStartScore());
    }
    emit updateTurn(game->getPlayerTurn());
    emit clearTakeRequest();
    emit signalPlayerUpdate();
}


void GameManager::changeNumOfPlayers(int number,QList<QSharedPointer<Player>> dbPlayerOrder){
    while(number<players.count()){
        players.removeLast();
    }
    while(number>players.count()){
        QSharedPointer<Player> player = dbPlayerOrder.first();
        for(QSharedPointer<Player> newplayer : dbPlayerOrder){
            bool notExisting = true;
            for(QSharedPointer<Player> exPl: players){
                if(newplayer->getID()==exPl->getID()){
                    notExisting=false;
                    break;
                }
            }
            if(notExisting){
                player=newplayer;
                break;
            }
        }
        player->reset(game->getStartScore());
        players.append(player);
    }

    game->reset();
    emit clearTakeRequest();
    emit signalGameStarted();
    emit signalPlayerUpdate();
}

void GameManager::handleUserDart(QPointF loc, QPointF intendedLoc){
    if(game->getStatus() != Game::IN_PROGRESS){
        continueGame();
        return;
    }
    game->handleDart(QSharedPointer<Dart>(new Dart(loc,intendedLoc)));
    players = game->getPlayers();
    emit signalPlayerUpdate();
    signalGameActive(isActive());
}

void GameManager::handleAIDartRequest(QPointF intendedLoc){
    QSharedPointer<Player> player = game->getCurrentPlayer();

    QPointF loc = dartBoard->aimDartAt(intendedLoc,player->getHDev(),player->getVDev());
    game->handleDart(QSharedPointer<Dart>(new Dart(loc,intendedLoc,true)));
    players = game->getPlayers();
    emit signalPlayerUpdate();
    signalGameActive(isActive());
}

void GameManager::throwAIDart(){
    QSharedPointer<Player> player =game->getCurrentPlayer();
    QPair<int,DartBoard::BED> target = game->generateTarget();
    QPointF intendedLoc = dartBoard->getCenterEstimate(target, player->getVDev());
    QPointF loc = dartBoard->aimDartAt(intendedLoc,player->getHDev(),player->getVDev());
    game->handleDart(QSharedPointer<Dart>(new Dart(loc,intendedLoc,true)));
    players = game->getPlayers();
    emit signalPlayerUpdate();
    signalGameActive(isActive());
}

void GameManager::addBounceOut(){
    game->handleDart(QSharedPointer<Dart>(new Dart(QPointF(-1,-1),QPointF())));
    players = game->getPlayers();
    emit signalPlayerUpdate();
    signalGameActive(isActive());
}



void GameManager::undoLastDart(){


    game->undoLastDart();


    emit updateTurn(game->getPlayerTurn());
    emit signalGameActive(isActive());
    emit signalPlayerUpdate();

    return;;
}

