#ifndef ADDEDITASIGURATOR_H
#define ADDEDITASIGURATOR_H

#include <QDialog>
#include <QNetworkReply>
#include <QMovie>
#include <QMessageBox>

#include "global.h"
#include "log.h"


namespace Ui {
    class AddEditAsigurator;
}

class AddEditAsigurator : public QDialog
{
    Q_OBJECT

signals:
    void kill();

public:
    explicit AddEditAsigurator(QWidget *parent = 0);
    ~AddEditAsigurator();

public slots:
    void setEditMode(QString asigurator, int tip, int asigurator_id);
    void setAddMode();

private:
    Ui::AddEditAsigurator *ui;
    bool add;
    QMovie *movie;
    int asigurator_id;

private slots:
    void loadingStart();
    void loadingStop();
    void handleNetworkError(QNetworkReply *reply);
    void addEdit();
    void parseAnswer(QByteArray data);

};

#endif // ADDEDITASIGURATOR_H
