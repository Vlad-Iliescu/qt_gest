#ifndef QTGEST_H
#define QTGEST_H

#include <QMainWindow>

#include "add/addcontract.h"
#include "mdis/mainmdi.h"
#include "edit/edit.h"
#include "todo/todo.h"
#include "furnizor/furnizori.h"
#include "asigurator/asiguratori.h"
#include "user/users.h"
#include "accelerate/alt_key.hpp"
#include "rapoarte/rapoarte.h"
#include "mdis/raport.h"
#include "mdis/finalizate.h"
#include "find/search.h"

#include "global.h"
#include "log.h"

namespace Ui {
    class QtGest;
}

class QtGest : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtGest(QWidget *parent = 0);
    ~QtGest();
    double getTVA();

public slots:
    void resetPrivileges();
    void setUsername(QString username);

private:
    Ui::QtGest *ui;
    double TVA;
    QLabel *loadingLabel;
    QLabel *username;
    QMovie *movie;
    QPushButton *todoButton;

    AddContract *addContract;
    MainMdi *mainMdi;
    ToDo *todo;
    QTimer *todoTimer;
    QTimer *refresh;

    void createLoadingLabel();
    void setUpMainMdi();

private slots:
    void setTva(double tva);

    void popupToDo();
    void parsePopup(QByteArray data);
    void handleNetworkError(QNetworkReply *reply);

    void adaugareContract();

    void editareContact(int contract_id);

    void loadingStart();
    void loadingStop();

    void logout();

    void furnizori();
    void asiguratori();
    void users();
    void rapoarte();
    void search();
    void raport(QVariantMap data);
    void finalizate();
};

#endif // QTGEST_H
