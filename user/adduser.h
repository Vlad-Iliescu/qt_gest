#ifndef ADDUSER_H
#define ADDUSER_H

#include <QDialog>
#include <QMovie>
#include <QNetworkReply>
#include <QCryptographicHash>

#include "global.h"
#include "log.h"

namespace Ui {
    class AddUser;
}

class AddUser : public QDialog
{
    Q_OBJECT

signals:
    void kill();

public:
    explicit AddUser(QWidget *parent = 0);
    ~AddUser();

private:
    Ui::AddUser *ui;
    QMovie *movie;

private slots:
    void loadingStart();
    void loadingStop();
    void addUser();
    void parseAnswer(QByteArray data);
    void handleNetworkError(QNetworkReply *reply);
};

#endif // ADDUSER_H
