#ifndef ADDCONTRACT_H
#define ADDCONTRACT_H

#include <QDialog>
#include <QUrlQuery>
#include "global.h"
#include "log.h"

namespace Ui {
    class AddContract;
}

class AddContract : public QDialog
{
    Q_OBJECT

signals:
    void added(int contract_id);
    void kill();

public:
    explicit AddContract(QWidget *parent = 0);
    ~AddContract();

private:
    Ui::AddContract *ui;
    QMovie *movie;
    QMap<QString, int> consilieri;

private slots:
    void addNewContract();
    void parseAnswer(QByteArray data);
    void handleNetworkError(QNetworkReply *reply);

    void getConsilieri();
    void parseConsilieri(QByteArray data);

    void loadingStart();
    void loadingStop();

};

#endif // ADDCONTRACT_H
