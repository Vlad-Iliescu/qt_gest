#ifndef FURNIZORI_H
#define FURNIZORI_H

#include <QDialog>
#include <QMovie>
#include <QMessageBox>
#include <QNetworkReply>
#include <QTreeWidgetItem>

#include "global.h"
#include "log.h"

#include "furnizor/addeditfrunizor.h"

namespace Ui {
    class Furnizori;
}

class Furnizori : public QDialog
{
    Q_OBJECT

signals:
    void kill();

public:
    explicit Furnizori(QWidget *parent = 0);
    ~Furnizori();

public slots:
    void getFurnizori();

private:
    Ui::Furnizori *ui;
    QMovie *movie;
    AddEditFrunizor *furnizor;

private slots:
    void loadingStart();
    void loadingStop();
    void handleNetworkError(QNetworkReply *reply);
    void parseFurnizori(QByteArray data);
    void parseDelete(QByteArray data);

    void addFunizor();
    void editFurnizor();
    void itemDC(QTreeWidgetItem *item, int col);
    void deleteFurnizor();

    void beforeKill();
};

#endif // FURNIZORI_H
