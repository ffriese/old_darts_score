#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLCDNumber>
#include <QListWidget>
#include "player.h"
#include "game.h"
#include <QSharedPointer>
#include <QPainter>
#include <QTableWidget>

class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    PlayerWidget(QWidget *parent, QSharedPointer<Game> _game, QList<QSharedPointer<Player>> players, QSharedPointer<Player> _player, int i);

    void setActive(bool active);
    void setTurn(bool turn);
    void update();
    void updatePlayers();

    QString getCurrentPlayerName(){
        return combobox->currentText();
    }

private:
    class LCDIndicator: public QLCDNumber {
            public:
                LCDIndicator(QWidget * parent): QLCDNumber(parent){}

                bool drawDot = false;
                QColor color = Qt::white;
            private:
                void paintEvent(QPaintEvent* event){
                    QLCDNumber::paintEvent(event);
                    if(drawDot){
                        QPainter painter(this);
                        painter.setBrush(QBrush(color));
                        painter.setPen(color);
                        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

                        int r = std::min<int>(this->width()/8,this->height()/5);
                        int x = 4;
                        int y = 4;

                        painter.drawEllipse(x,y,r,r);
                    }
                }
          };

    QSharedPointer<Player> player;
    QSharedPointer<Game> currentGame;
    QLabel* label;
    QComboBox* combobox;
    QComboBox* handicapCombobox;

    QLabel* scoreLb;
    QLabel* setLb;
    QLabel* legLb;
    LCDIndicator* score;
    LCDIndicator* sets;
    LCDIndicator* legs;
    QListWidget* takes;
    QTableWidget* cricketScoreSheet;
    QMap<QListWidgetItem*,QSharedPointer<Take>> itemTakes;

    bool left;
    void paintEvent(QPaintEvent *);

private slots:
    void handleTakeSelection(QListWidgetItem* item);

signals:
    void takeSelected(QSharedPointer<Take> take);
    void playerSelected(QString name);

public slots:
};

#endif // PLAYERWIDGET_H
