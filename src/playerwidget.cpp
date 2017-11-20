#include "playerwidget.h"
#include <QPainter>
#include <QGridLayout>
#include <QFont>
#include <QFontDatabase>
#include <QDebug>
#include <QCoreApplication>
#include <QHeaderView>
#include "dbmanager.h"
#include "cricket.h"

PlayerWidget::PlayerWidget(QWidget *parent, QSharedPointer<Game> _game, QList<QSharedPointer<Player>> players,QSharedPointer<Player> _player, int i) : QWidget(parent)
{




    currentGame = _game;
    player = _player;

    left = (i%2 == 0);
    int scoreW = 2;

    QPalette p = this->palette();
  //  p.setColor(QPalette::Background,QColor(20,80,20));
  //  p.setColor(QPalette::Text,QColor(255,255,255));
    p.setColor(QPalette::Background,QColor(31,78,120));
    p.setColor(QPalette::Text,QColor(155,194,230));
    this->setPalette(p);
    this->setAutoFillBackground(true);
    QGridLayout* lay = new QGridLayout();
    lay->setMargin(2);
    this->setLayout(lay);


    scoreLb = new QLabel("SCORE",this);
    scoreLb->setFont(QFont("Calibri",8));
    scoreLb->setStyleSheet("color:white");
    setLb = new QLabel("SETS (of "+QString::number(currentGame->getSetsToWin())+")",this);
    setLb->setFont(QFont("Calibri",8));
    setLb->setStyleSheet("color:white");
    legLb = new QLabel("LEGS (of "+QString::number(currentGame->getLegsToWin())+")" ,this);
    legLb->setFont(QFont("Calibri",8));
    legLb->setStyleSheet("color:white");


    score = new LCDIndicator(this);
    score->setDigitCount(3);
    score->setSegmentStyle(QLCDNumber::Flat);
    score->setFrameStyle(QFrame::StyledPanel);
    score->setFrameShadow(QFrame::Sunken);
    score->setStyleSheet("background-color:black; color:red");

    sets = new LCDIndicator(this);
    sets->setDigitCount(2);
    sets->setSegmentStyle(QLCDNumber::Flat);
    sets->setFrameStyle(QFrame::StyledPanel);
    sets->setFrameShadow(QFrame::Sunken);
    sets->setStyleSheet("background-color:black; color:rgb(85,160,245)");

    legs = new LCDIndicator(this);
    legs->setDigitCount(2);
    legs->setSegmentStyle(QLCDNumber::Flat);
    legs->setFrameStyle(QFrame::StyledPanel);
    legs->setFrameShadow(QFrame::Sunken);
    legs->setStyleSheet("background-color:black; color:rgb(85,160,245)");

    if(currentGame->getSetsToWin()<2){
        sets->setVisible(false);
        setLb->setVisible(false);
        scoreW++;
    }
    if(currentGame->getLegsToWin()<2){
        legs->setVisible(false);
        legLb->setVisible(false);
        scoreW++;
    }


    // LABEL
    label = new QLabel(player->getName(),this);

    int id_name = QFontDatabase::addApplicationFont(":/fonts/fonts/Calibri.ttf");
    int id_chalk = QFontDatabase::addApplicationFont(":/fonts/fonts/LHandW.ttf");
    QString family_name, family_chalk;
    if(id_name!=-1){
        family_name = QFontDatabase::applicationFontFamilies(id_name).at(0);
    }else{
        family_name="Arial";
    }
    if(id_chalk!=-1){
        family_chalk = QFontDatabase::applicationFontFamilies(id_chalk).at(0);
    }else{
        family_chalk="Arial";
    }


    double fontSize = parentWidget()->geometry().height();
    qDebug()<<fontSize;

    label->setStyleSheet("QLabel { color : rgb(155,194,230); }");
    label->setFont(QFont(family_name,fontSize));
    label->setVisible(false);

    if(!left){
        label->setAlignment(Qt::AlignRight);
    }

    //COMBOBOX
    combobox = new QComboBox(this);
    combobox->setVisible(true);
    combobox->setFont(label->font());

    for(QSharedPointer<Player> p: players){
       combobox->addItem(p->getName());
    }
    combobox->setCurrentText(player->getName());
    connect(combobox,SIGNAL(currentTextChanged(QString)),this,SIGNAL(playerSelected(QString)));


  /*  handicapCombobox = new QComboBox(this);
    handicapCombobox->setVisible(true);
    handicapCombobox->setFont(label->font());

    for(Game::HANDICAP h: currentGame->getAvailableHandicaps()){
       handicapCombobox->addItem(QString::number(h));
    }
*/

    lay->addWidget(label, 0, 0, 1, 4);
    lay->addWidget(combobox, 0, 0, 1, 4);
    lay->addWidget(setLb, 1, 0, 1, 1);
    lay->addWidget(legLb, 1, currentGame->getSetsToWin()<2 ? 0 : 1, 1, 1);
    lay->addWidget(scoreLb, 1, 4-scoreW, 1, scoreW);
    lay->addWidget(sets, 2, 0, 1, 1);
    lay->addWidget(legs, 2, currentGame->getSetsToWin()<2 ? 0 : 1, 1, 1);
    lay->addWidget(score, 2, 4-scoreW, 1, scoreW);

    if(currentGame->getGameName() == "Cricket"){
        cricketScoreSheet = new QTableWidget(7,1,this);
        cricketScoreSheet->viewport()->setFocusPolicy(Qt::NoFocus);
        cricketScoreSheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
        cricketScoreSheet->setSelectionMode(QAbstractItemView::NoSelection);
        cricketScoreSheet->setFont(QFont("Symbola",12));
        cricketScoreSheet->setStyleSheet("background-color: rgb(20,80,20); color: rgb(255,255,255)");
        cricketScoreSheet->horizontalHeader()->hide();
        cricketScoreSheet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        cricketScoreSheet->verticalHeader()->setSectionResizeMode (QHeaderView::Fixed);
        cricketScoreSheet->verticalHeader()->setStyleSheet("QHeaderView::section { background-color: rgb(20,80,20); color: rgb(255,255,255) }");
        cricketScoreSheet->verticalHeader()->setFont(cricketScoreSheet->font());
        //  cricketScoreSheet->insertColumn(0);
        for(int row=0;row<7;row++){
            //    cricketScoreSheet->insertRow(row);
            cricketScoreSheet->setItem(row,0,new QTableWidgetItem(""));
        }

        cricketScoreSheet->setVerticalHeaderLabels({"20","19","18","17","16","15","B"});


        lay->addWidget(cricketScoreSheet, 3, 0, 1, 4);
    }else{
        takes = new QListWidget(this);
        //takes->setPalette(Qt::black);
        takes->setFont(QFont(family_chalk,12));
        takes->setStyleSheet("background-color: rgb(20,80,20); color: rgb(255,255,255)");
        //p.setColor(QPalette::Text,QColor(255,255,255));
        takes->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(takes,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(handleTakeSelection(QListWidgetItem*)));
        //connect(takes,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(listMenuRequested(QPoint)));
        lay->addWidget(takes, 3, 0, 1, 4);
    }



}


void PlayerWidget::setActive(bool active){
    label->setVisible(active);
    combobox->setVisible(!active);
}


void PlayerWidget::handleTakeSelection(QListWidgetItem* item){
    QSharedPointer<Take> take =itemTakes.value(item);
  //  qDebug()<<"selected item"<<item<<"with take"<<take;
    emit takeSelected(take); //TODO: clear all other selections
    item->setSelected(true);
}

void PlayerWidget::updatePlayers(){
    QString cur= combobox->currentText();
    combobox->clear();
    for(QSharedPointer<Player> p: DBManager::getInstance()->getPlayers(true)){
        combobox->addItem(p->getName());
    }
    combobox->setCurrentText(cur);
}

void PlayerWidget::update(){
    if(currentGame->getGameName()=="Cricket"){

     Cricket* cricket_game = (Cricket*) currentGame.data();
        QMap<int, int> hits = cricket_game->getHitCounter(player);

        QList<int> order= {20,19,18,17,16,15,25};
        for(int field: hits.keys()){
            //qDebug()<<cricketScoreSheet->item(row,0);
            int numhits = hits.value(field);
            QString lb="";
            switch(numhits){
            case 0:
                break;
            case 1:
                lb=" ̷";
                break;
            case 2:
                lb="×";
                break;
            case 3:
                lb="⦻";
                break;
            default:
                lb="⦻ ";
                for(int i=numhits; i>3; i--){
                    lb.append("|"); //⧺ ⧻
                }
                break;
            }

            cricketScoreSheet->item(order.indexOf(field),0)->setText(lb);
        }

    }else{


        for(QListWidgetItem* item: itemTakes.keys()){
            delete item;
        }
        itemTakes.clear();
        takes->clear();
        for(QSharedPointer<Take> take: player->getTakes()){
            QString takeText= (take->getResult()==Take::BUST) ? "BUST" : QString::number(take->getScore());
            takeText.append(" [");
            for(int i = 0; i<take->getDarts().count();i++){
                takeText.append(take->getDarts().at(i)->getString());
                if(i<take->getDarts().count()-1)
                    takeText.append(",");
            }
            takeText.append("]");

            QListWidgetItem* item =new QListWidgetItem(takeText);
            itemTakes.insert(item,take);
            takes->addItem(item);
        }

        if(player->getDartsLeft()>0 ){
            QSharedPointer<Take> curTake= player->getCurrentTake();
            QString takeText= (curTake->getResult()==Take::BUST) ? "BUST" : QString::number(curTake->getScore());
            takeText.append(" [");
            for(int i = 0; i<curTake->getDarts().count();i++){
                takeText.append(curTake->getDarts().at(i)->getString());
                if(i<curTake->getDarts().count()-1)
                    takeText.append(",");
            }
            takeText.append(" ]");
           // takeText.append(" ("+QString::number(player->getDartsLeft())+" darts left)");

            QListWidgetItem* item =new QListWidgetItem(takeText);
            item->setForeground(Qt::white); //
            item->setBackground(Qt::darkGreen); //
            itemTakes.insert(item,curTake);
            takes->addItem(item);
        }
        takes->scrollToBottom();
    }
    score->display(currentGame->getLiveScore(player));
    sets->display(player->getWonSets());
    legs->display(player->getWonLegs());

    QColor color = Qt::red;
    if(currentGame->getCurrentLegBeginner() == player){
        //QColor color = QColor(0,255,0);
       // QColor color = QColor(85,160,245);
        legs->setStyleSheet("background-color: black; color: rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")");
        legs->color = color;
        legs->drawDot = true;
    }else{
       // QColor color = QColor(85,160,245);
        legs->setStyleSheet("background-color: black; color: rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")");
        //legs->color = color;
        legs->drawDot = false;
    }
    if(currentGame->getCurrentSetBeginner() == player){
        //QColor color = QColor(0,255,0);
        //QColor color = QColor(85,160,245);
        sets->setStyleSheet("background-color: black; color: rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")");
        sets->color = color;
        sets->drawDot = true;
    }else{
        //QColor color = QColor(85,160,245);
        sets->setStyleSheet("background-color: black; color: rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")");
       // sets->color = color;
        sets->drawDot = false;
    }

    QString tooltip = "Game 3-Dart-Avg.: "+QString::number(player->getGameAvg()*3)+"\n"+
            "Game Avg. Distance: "+QString::number(player->getGameStdDev(),'g',3)+" cm\n\n"+
            "Total 3-Dart-Avg.: "+QString::number(DBManager::getInstance()->getAverage(player))+"\n"+
            "Total HDev: "+QString::number(player->getHDev()/10,'g',3)+" cm\n"+
            "Total VDev: "+QString::number(player->getVDev()/10,'g',3)+" cm";
    combobox->setToolTip(tooltip);
    label->setToolTip(tooltip);

    repaint();
}

void PlayerWidget::paintEvent(QPaintEvent *){



    double fontSize = std::max<double>(parentWidget()->parentWidget()->geometry().height()/40.0 , 8);
    QFont font = label->font();
    font.setPointSize(fontSize);
    label->setFont(font);
    combobox->setFont(font);

    if(currentGame->getGameName() == "Cricket"){

    }else{
        font = takes->font();
        font.setPointSize(std::max<double>(fontSize/1.7,8.0));
        takes->setFont(font);

    }
    combobox->setMaximumHeight(fontSize*2);
    score->setMinimumHeight(fontSize*2);
    sets->setMinimumHeight(fontSize*2);
    legs->setMinimumHeight(fontSize*2);




    for(int i=0; i<player->getDartsLeft(); i++){
       QPainter painter(this);
       painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
       int turnId_x = left ? this->width()-35: 0;
       painter.drawImage(turnId_x+i*8,label->geometry().height()/2-10,QImage(":/images/img/d1.png"));
    }





}
