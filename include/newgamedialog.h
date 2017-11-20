#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include "game.h"

namespace Ui {
class NewGameDialog;
}

class NewGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGameDialog(QWidget *parent = 0);
    ~NewGameDialog();

    void setSelectedGame(QSharedPointer<Game> game);

signals:
    void gameCreated(QSharedPointer<Game> game);

private:


    Ui::NewGameDialog *ui;
    QSharedPointer<Game> currentGame;
    QMap<QString, Game::VARIANT> varMap;
    QList<QComboBox*> varOptions;

private slots:
    void updateVariants(QList<Game::VARIANT> defaults = {});

    void on_buttonBox_accepted();
};

#endif // NEWGAMEDIALOG_H
