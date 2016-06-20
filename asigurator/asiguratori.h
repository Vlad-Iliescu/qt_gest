#ifndef ASIGURATORI_H
#define ASIGURATORI_H

#include <QDialog>
#include <QMovie>
#include <QMessageBox>
#include <QNetworkReply>
#include <QTreeWidgetItem>

#include "global.h"
#include "log.h"

#include "asigurator/addeditasigurator.h"

namespace Ui {
    class Asiguratori;
}

class Asiguratori : public QDialog
{
    Q_OBJECT

signals:
    void kill();


public:
    explicit Asiguratori(QWidget *parent = 0);
    ~Asiguratori();

public slots:
    void getAsiguratori();

private:
    Ui::Asiguratori *ui;
    QMovie *movie;
    AddEditAsigurator *asigurator;

private slots:
    void loadingStart();
    void loadingStop();
    void handleNetworkError(QNetworkReply *reply);
    void parseAsiguratori(QByteArray data);
    void parseDelete(QByteArray data);

    void addAsigurator();
    void editAsigurator();
    void itemDC(QTreeWidgetItem *item, int col);
    void deleteAsigurator();

    void beforeKill();
};

#endif // ASIGURATORI_H
