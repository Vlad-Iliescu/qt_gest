#ifndef RAPOARTE_H
#define RAPOARTE_H

#include <QDialog>
#include <QNetworkReply>
#include <QMovie>
#include <QMessageBox>
#include <QTreeWidgetItem>

#include "rapoarte/addraport.h"

#include "global.h"
#include "log.h"

namespace Ui {
    class Rapoarte;
}

class Rapoarte : public QDialog
{
    Q_OBJECT

signals:
    void error(QString message);
    void kill();
    void raportReady(QVariantMap data);

public:
    explicit Rapoarte(QWidget *parent = 0);
    ~Rapoarte();

public slots:
    void getRapoarte();
    void resetPrivileges();

private slots:
    void parseRapoarte(QByteArray data);
    void handleNetworkError(QNetworkReply *reply);

    void loadingStart();
    void loadingStop();

    void getRaport(QTreeWidgetItem *item = 0, int col = 0);
    void parseRaport(QByteArray data);

    void queryResult(QString SQLQuery);
    void stergereRaport();
    void parseAnswer(QByteArray data);

    void addNew();
    void editRaport();
    void viewRaport(QByteArray data);

    void beforeKill();

private:
    Ui::Rapoarte *ui;
    QMovie *movie;
    AddRaport *addraport;
};

#endif // RAPOARTE_H
