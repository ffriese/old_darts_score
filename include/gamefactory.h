#ifndef GAMEFACTORY
#define GAMEFACTORY

#include "game.h"
#include "x01.h"
#include "cricket.h"
#include "roundtheclock.h"

template<typename T> Game* createInstance(int _legsToWin, QList<Game::VARIANT> _variants,int _gameID, int _setsToWin) {
    return new T(_legsToWin,_variants,_gameID,_setsToWin);
}


typedef QMap<QString, Game*(*)(int _legsToWin, QList<Game::VARIANT> _variants, int _gameID, int _setsToWin)> ConstructorMap;



class GameFactory
{
public:

    GameFactory(){
            constructors.clear();
            constructors.insert("X01",&createInstance<X01>);
            constructors.insert("Cricket",&createInstance<Cricket>);
            constructors.insert("Round The Clock",&createInstance<RoundTheClock>);
        }

        static GameFactory* getInstance(){
            static GameFactory* instance;
            if(instance==NULL){
                instance = new GameFactory();
            }
            return instance;
        }

        QSharedPointer<Game> createGame(QString _gameName, int _legsToWin, QList<Game::VARIANT> _variants,int _gameID = -1, int _setsToWin = 1){
            static QSharedPointer<Game> gm;

            if(_gameName.endsWith("01")){
                switch(_gameName.left(1).toInt()){
                case 9:
                    _variants.append(Game::VARIANT::X01_901);
                    break;
                case 7:
                    _variants.append(Game::VARIANT::X01_701);
                    break;
                case 5:
                    _variants.append(Game::VARIANT::X01_501);
                    break;
                case 1:
                    _variants.append(Game::VARIANT::X01_101);
                    break;
                case 3:
                default:
                    _variants.append(Game::VARIANT::X01_301);
                    break;
                }
                _gameName="X01";
            }

            gm.reset(constructors[_gameName](_legsToWin,_variants, _gameID, _setsToWin));
            return gm;
        }


        QList<QString> getAvailableGames(){
            return {
                "101",
                "301",
                "501",
                "701",
                "901",
                "Cricket",
                "Round The Clock"
            };
        }

private:
    ConstructorMap constructors;


};
#endif // GAMEFACTORY

