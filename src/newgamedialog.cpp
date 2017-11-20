#include "newgamedialog.h"
#include "ui_newgamedialog.h"
#include "gamefactory.h"
#include <QGridLayout>

NewGameDialog::NewGameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGameDialog)
{
    ui->setupUi(this);
}


void NewGameDialog::setSelectedGame(QSharedPointer<Game> game){

    QGridLayout* lay = new QGridLayout(ui->groupBox);
    ui->groupBox->setLayout(lay);

    for(QString g_name: GameFactory::getInstance()->getAvailableGames()){
        ui->comboBox->addItem(g_name);
    }


    ui->spinBoxLegs->setValue(game->getLegsToWin());
    ui->spinBoxSets->setValue(game->getSetsToWin());
    QString curName = game->getGameName();
    if(curName == "X01")
        curName = QString::number(game->getStartScore());

    ui->comboBox->setCurrentText(curName);


    connect(ui->comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updateVariants()));
    updateVariants(game->getVariants());


}

void NewGameDialog::updateVariants(QList<Game::VARIANT> defaults){


    for(QObject* c: ui->groupBox->children()){
        c->deleteLater();
    }

    QGridLayout* lay = (QGridLayout*) ui->groupBox->layout();


    currentGame = GameFactory::getInstance()->createGame(ui->comboBox->currentText(),1,{});
    QMap<QString,QList<Game::VARIANT>> optionGroups = currentGame->getOptionGroups();

    varMap.clear();
    varOptions.clear();
    for(QString option: optionGroups.keys()){
        int row = lay->rowCount();
        lay->addWidget(new QLabel(option,ui->groupBox),row,0);
        QComboBox* var_select = new QComboBox(ui->groupBox);
        varOptions.append(var_select);
        lay->addWidget(var_select,row,1);
        for(Game::VARIANT var: optionGroups.value(option)){
            QString var_str = currentGame->getVariantString(var);
            varMap.insert(var_str,var);
            var_select->addItem(var_str);
        }

        for(Game::VARIANT var: optionGroups.value(option)){
            if((defaults.isEmpty() && currentGame->isDefaultVariant(var) ) ||
                    defaults.contains(var)){
                var_select->setCurrentText(currentGame->getVariantString(var));
                break;
            }
        }
    }
}

NewGameDialog::~NewGameDialog()
{
    delete ui;
}

void NewGameDialog::on_buttonBox_accepted()
{
    QList<Game::VARIANT> selectedVars;
    for(QComboBox* varOpt :varOptions){
        selectedVars.append(varMap.value(varOpt->currentText()));
    }
    currentGame = GameFactory::getInstance()->createGame(ui->comboBox->currentText(),
                                                         ui->spinBoxLegs->value(),
                                                         selectedVars,-1,
                                                         ui->spinBoxSets->value());
    emit gameCreated(currentGame);
}
