#ifndef EDITUSER_H
#define EDITUSER_H

#include <QDialog>
#include <QMovie>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QMessageBox>

#include "global.h"
#include "log.h"

namespace Ui {
    class EditUser;
}

class EditUser : public QDialog
{
    Q_OBJECT

signals:
    void kill();

public:
    explicit EditUser(QWidget *parent = 0);
    ~EditUser();
    void setUserId(int id, QString username, int nivel, QString name, QString email);

private:
    Ui::EditUser *ui;
    int user_id;
    QMovie *movie;

private slots:
    void loadingStart();
    void loadingStop();
    void editUser();
    void parseAnswer(QByteArray data);
    void handleNetworkError(QNetworkReply *reply);
};

#endif // EDITUSER_H
