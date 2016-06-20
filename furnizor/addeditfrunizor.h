#ifndef ADDEDITFRUNIZOR_H
#define ADDEDITFRUNIZOR_H

#include <QDialog>
#include <QNetworkReply>
#include <QMovie>
#include <QMessageBox>

#include "global.h"
#include "log.h"

namespace Ui {
    class AddEditFrunizor;
}

class AddEditFrunizor : public QDialog
{
    Q_OBJECT

signals:
    void kill();

public:
    explicit AddEditFrunizor(QWidget *parent = 0);
    ~AddEditFrunizor();

public slots:
    void setEditMode(QString furnizor, int furnizor_id);
    void setAddMode();

private:
    Ui::AddEditFrunizor *ui;
    bool add;
    QMovie *movie;
    int furnizor_id;

private slots:
    void loadingStart();
    void loadingStop();
    void handleNetworkError(QNetworkReply *reply);
    void addEdit();
    void parseAnswer(QByteArray data);

};

#endif // ADDEDITFRUNIZOR_H
