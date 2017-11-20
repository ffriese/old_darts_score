#include "manageplayersdialog.h"
#include "ui_manageplayersdialog.h"
#include "dbmanager.h"
#include <QDebug>
#include <QMessageBox>

ManagePlayersDialog::ManagePlayersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManagePlayersDialog)
{
    ui->setupUi(this);


    for(QSharedPointer<Player> p: DBManager::getInstance()->getPlayers(true)){
        ui->listWidget->addItem(p->getName());
        players.append(p->getName());
    }
}

ManagePlayersDialog::~ManagePlayersDialog()
{
    delete ui;
}

void ManagePlayersDialog::on_addPlayerButton_clicked()
{
    QString newPlayer = ui->lineEdit->text();
    if(newPlayer == ""){
        return;
    }
    if(players.contains(newPlayer)){
        qDebug()<<"name already taken";
        return;
    }

    QMessageBox::StandardButton reply;

   // reply = QMessageBox::question(this, "Add Player?","Do you want to add "+newPlayer+" to the database?",
   //                                 QMessageBox::Yes|QMessageBox::No);
   // if(reply==QMessageBox::Yes){
        bool success = DBManager::getInstance()->addPlayer(newPlayer);
        ui->listWidget->addItem(newPlayer);
        players.append(newPlayer);
        newPlayers.append(newPlayer);
  //  }
}
