#ifndef MANAGEPLAYERSDIALOG_H
#define MANAGEPLAYERSDIALOG_H

#include <QDialog>

namespace Ui {
class ManagePlayersDialog;
}

class ManagePlayersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManagePlayersDialog(QWidget *parent = 0);
    ~ManagePlayersDialog();

private slots:
    void on_addPlayerButton_clicked();

private:
    Ui::ManagePlayersDialog *ui;
    QList<QString> players;
    QList<QString> newPlayers;
};

#endif // MANAGEPLAYERSDIALOG_H
