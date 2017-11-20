#include "statviewer.h"
#include "ui_statviewer.h"
#include <QDebug>

StatViewer::StatViewer(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::StatViewer)
{
    ui->setupUi(this);
    playerMap.clear();

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(executeQuery()));

    players = DBManager::getInstance()->getPlayers();
    foreach(QSharedPointer<Player> player, players){
       ui->comboBox->addItem(player->getName());
       playerMap.insert(player->getName(),player);
    }
    ui->comboBox->setCurrentText(players.first()->getName());

    ui->dateEdit->setDate(DBManager::getInstance()->getEarliestDate());
    ui->dateEdit_2->setDate(QDate::currentDate());

    ui->dateEdit->setMinimumDate(DBManager::getInstance()->getEarliestDate());
    ui->dateEdit_2->setMinimumDate(DBManager::getInstance()->getEarliestDate());
    ui->dateEdit->setMaximumDate(QDate::currentDate());
    ui->dateEdit_2->setMaximumDate(QDate::currentDate());

    ui->comboBox_2->addItem("Average");
    ui->comboBox_2->addItem("Horizontal Deviation");
    ui->comboBox_2->addItem("Vertical Deviation");

}

StatViewer::~StatViewer()
{
    delete ui;
}


void StatViewer::executeQuery(){

    qDebug()<<playerMap;
    QSharedPointer<Player> pl = playerMap.value(ui->comboBox->currentText());
    double val;
    switch(ui->comboBox_2->currentIndex()){
    case 0:
        val = DBManager::getInstance()->getAverage(pl,ui->dateEdit->date(),ui->dateEdit_2->date());
        break;
    case 1:
        val = DBManager::getInstance()->getHDev(pl,ui->dateEdit->date(),ui->dateEdit_2->date());
        break;
    case 2:
        val = DBManager::getInstance()->getVDev(pl,ui->dateEdit->date(),ui->dateEdit_2->date());
        break;
    default:
        val = DBManager::getInstance()->getAverage(pl,ui->dateEdit->date(),ui->dateEdit_2->date());
        break;
    }

    ui->lcdNumber->display(val);
}
