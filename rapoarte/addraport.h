#ifndef ADDRAPORT_H
#define ADDRAPORT_H

#include <QDialog>
#include <QInputDialog>
#include <QNetworkReply>
#include <QMovie>
#include <QMessageBox>

#include "QCheckList/checkboxlist.h"

#include "global.h"
#include "log.h"

#include "math.h"

#include "accelerate/alt_key.hpp"

namespace Ui {
    class AddRaport;
}

class AddRaport : public QDialog
{
    Q_OBJECT

signals:
    void error(QString message);
    void kill();
    void query(QByteArray query);
    void rapoarteChanged();

public:
    explicit AddRaport(QWidget *parent = 0);
    void getEditData(int raport_id);
    ~AddRaport();

public slots:
    void resetPrivileges();

private slots:
    void getData();
    void parseData(QByteArray data);
    void parseEditData(QByteArray data);
    void handleNetworkError(QNetworkReply *reply);
    void itemCheckChenged(QListWidgetItem *item);

    void saveRaport();
    void parseAnswer(QByteArray data);
    void viewRaport();

    void loadingStart();
    void loadingStop();

    QVariantList getColumns();
    QVariantList getHiddenColumns();
    QVariantList getDiabledWidgets();
    QVariantList getActiveWidgets();

    void checkAll();
    void uncheckAll();

    void activeCheckAll();
    void activeUncheckAll();

private:
    Ui::AddRaport *ui;
    QMovie *movie;
    QMap< QString, QString > cols;
    QHash< int, QString > users, cons;
    bool visible;
    bool getting_data;
    int raport_id;

    void createConnections();
    inline double round(double n, unsigned d = 2) {
          return floor(n * pow(10., d) + .5) / pow(10., d);
    }

};

#endif // ADDRAPORT_H
