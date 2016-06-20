#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QMovie>


namespace Ui {
    class Login;
}

class Login : public QDialog
{
    Q_OBJECT

signals:
    void error(QString message);

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    int getNivel();
    QString getUsername();
    QMovie *movie;

private:
    Ui::Login *ui;
    int nivel; // 1 = user, 2 = admin
    QString username;

private slots:
    void logare();
    void loadingStart();
    void loadingStop();
    void parseLogin(QByteArray data);
    void parseError(QNetworkReply *reply);
    void parseEcho(QNetworkReply *reply);

};

#endif // LOGIN_H
