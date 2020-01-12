#include "game.h"
#include <QVector2D>
#include "dbmanager.h"
#include <QDebug>
#include <QSound>
#include <QAudioDeviceInfo>
#include "globalconfig.h"

Game::Game(int _legsToWin, QList<VARIANT> _variants, int _gameID, int _setsToWin)
{
    if(_gameID == -1){
        gameID=DBManager::getInstance()->getNextGameID();
    }else{
        gameID =_gameID;
    }
    legsToWin = _legsToWin;
    setsToWin = _setsToWin;
    variants = _variants;
    //legs.clear();
    sets.clear();
    status = WAITING_FOR_USER_INPUT;
}

void Game::start(QList<QSharedPointer<Player>> _players,
                 QMap<QSharedPointer<Player>,QList<HANDICAP> > _handicaps,
                 int _startingPlayer){
    status = IN_PROGRESS;
    for(QSharedPointer<Player> p:_players){
        qDebug()<<p->getName();
    }

    players.clear();
    players.append(_players);
    qDebug()<<players.count();
    playerTurn = _startingPlayer;
    getCurrentPlayer()->setDartsLeft(3);
    qDebug()<<getCurrentPlayer()->getName()<<":"<<getCurrentPlayer()->getDartsLeft()<<"darts left"<<getCurrentPlayer().data();
    sets.append(QSharedPointer<Set>(new Set(players.first(),legsToWin)));
    sets.last()->addLeg(players.first());
    init();
}


void Game::continueGame(){
    switch(status){
    case LEG_FINNISHED:
        status = IN_PROGRESS;
        qDebug()<<"continueing game";
        nextLeg();
        break;
    default:
        qDebug()<<"do nothing";
        break;
    }
}


int Game::getNextPlayerIdx(int pIdx){
    if(pIdx==players.count()-1){
        return 0;
    }else{
        return pIdx+1;
    }
}


void Game::nextLeg(){
    if(sets.last()->getWinner()){
        playerTurn = getNextPlayerIdx(players.indexOf(sets.last()->getBeginner()));
        sets.append(QSharedPointer<Set>(new Set(getCurrentPlayer(),legsToWin)));
    }else{
        playerTurn = getNextPlayerIdx(players.indexOf(sets.last()->getCurrentLeg()->getBeginner()));
    }
    sets.last()->addLeg(getCurrentPlayer());
    for(QSharedPointer<Player> player: players){
        player->nextLeg(getStartScore());
        player->setDartsLeft(0);
    }
    getCurrentPlayer()->setDartsLeft(3);
}


void Game::playScoreSound(int score, bool req){
    if(!GlobalConfig::SOUND_ENABLED){
        return;
    }
    QString num = QString::number(score);
    QString soundfile = ":/sounds/sounds/Announcer/";
    if(req){
        return; //REMOVE FOR 'YOU REQUIRE', TODO: DELAY
        if(score>180){
            return;
        }
        soundfile = soundfile + "yr_" + num + ".wav";
    }else{
        if(score==180){
            int r = ((double)qrand()/(double)RAND_MAX)*3 +1;
            num = num + "v" + QString::number(r);
        }
        soundfile = soundfile + "an_" + num + ".wav";
    }
    QSound::play(soundfile);
}


void Game::playDartHitSound(){
    return; // DISABLE HITSOUND FOR NOW
    if(!GlobalConfig::SOUND_ENABLED){
        return;
    }
    QString soundfile = ":/sounds/sounds/Darts/";
    int r = ((double)qrand()/(double)RAND_MAX)*3 +1;
    soundfile = soundfile + "Hit_" + QString::number(r) + ".wav";
    QSound::play(soundfile);
}


Game::DARTRESULT Game::handleDart(QSharedPointer<Dart> dart){

    //qDebug()<<getCurrentPlayer()->getName()<<":"<<getCurrentPlayer()->getDartsLeft()<<"darts left"<<getCurrentPlayer().data();
    QPair<int,DartBoard::BED> result = DartBoard::getInstance()->getThrowResult(dart->getExactLocation());
    int score = DartBoard::getInstance()->calcScore(result);

    QString dbg_out;
    dbg_out =players.at(playerTurn)->getName()+" hit ";
    dbg_out.append(DartBoard::getInstance()->getFieldName(result));

    if(!dart->getIntendedLoctaion().isNull()){
        QVector2D distVec(dart->getExactLocation()-dart->getIntendedLoctaion());
        double dist = distVec.length()/10;
        dbg_out.append(" while aiming for ");
        dbg_out.append(DartBoard::getInstance()->getFieldName(DartBoard::getInstance()->getThrowResult(dart->getIntendedLoctaion())));
        dbg_out.append(", dist: ");
        dbg_out.append(QString::number(dist));
        dbg_out.append(" cm");
    }

    dbg_out.append(" -> ");
    dbg_out.append("score: "+QString::number(score));

    bool nobust = addDartToTake(players.at(playerTurn),QSharedPointer<Dart>(dart));
    if(!nobust){
        dbg_out.append(" -> BUST! Reset Score.");
    }

    dbg_out.append(" -> Required: "+QString::number(getLiveScore(players.at(playerTurn))));

    qDebug()<<dbg_out;
    checkTurn();

    switch(status){
    case LEG_FINNISHED:
    case FINNISHED:
        return DARTRESULT::WIN;
    default:
        return DARTRESULT::NONE;
    }
}

void Game::undoLastDart(){
    if(status != IN_PROGRESS){
        int winner = getWinner();
        if(winner<0){
            return;
        }
        playerTurn=winner;
        players.at(playerTurn)->undoLastDart();
        return;
    }
    int dartsLeft=players.at(playerTurn)->getDartsLeft();
    if(dartsLeft<3 && dartsLeft>0){
        players.at(playerTurn)->undoLastDart();
    }else if(dartsLeft==3){
        int prevIdx = playerTurn==0 ? prevIdx=players.count()-1 : playerTurn-1;
        if(players.at(prevIdx)->getTakes().count()>0){
            players.at(playerTurn)->setDartsLeft(0);
            playerTurn=prevIdx;
            players.at(playerTurn)->undoLastDart();
        }
    }
}
