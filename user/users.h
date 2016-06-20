#ifndef USERS_H
#define USERS_H

#include <QDialog>
#include <QMovie>
#include <QNetworkReply>
#include <QTreeWidgetItem>

#include "global.h"
#include "log.h"

#include "adduser.h"
#include "edituser.h"

namespace Ui {
    class Users;
}

class Users : public QDialog
{
    Q_OBJECT

signals:
    void kill();

public:
    explicit Users(QWidget *parent = 0);
    ~Users();
    void getUsers();

private:
    Ui::Users *ui;
    QMovie *movie;
    AddUser *addUserDlg;
    EditUser *editUserDlg;

private slots:
    void loadingStart();
    void loadingStop();
    void parseUsers(QByteArray data);
    void parseAnswer(QByteArray data);
    void handleNetworkError(QNetworkReply *reply);
    void editUser(QTreeWidgetItem *item, int col = 0);
    void editUserButton();
    void addUser();
    void deleteUser();

    void beforeKill();
};

#endif // USERS_H
