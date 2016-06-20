#ifndef MAINMDI_H
#define MAINMDI_H

#include <QWidget>
#include <QCloseEvent>
#include <QHeaderView>
#include <QUrlQuery>

#include "math.h"

#include "global.h"
#include "log.h"

namespace Ui {
    class MainMdi;
}

class MainMdi : public QWidget
{
    Q_OBJECT

signals:
    void loadingStart();
    void loadingStop();
    void tva(double);
    void kill();
    void editContract(int contract_id);

public:
    explicit MainMdi(QWidget *parent = 0);
    ~MainMdi();

public slots:
    void getContent();
    void resetPrivileges();


private:
    Ui::MainMdi *ui;
    void closeEvent(QCloseEvent *event);
    QMenu menu;
    QAction *editeaza;
    QAction *finalizare;
    QHash<int,QString> users;

    inline double round(double n, unsigned d = 2) {
          return floor(n * pow(10., d) + .5) / pow(10., d);
    }

private slots:
    void handleNetworkError(QNetworkReply *reply);

    void parseContent(QByteArray data);
    void parseAnswer(QByteArray data);

    void showCustomContextMenu(QPoint point);

    void createActions();
    void editeazaContract(QTreeWidgetItem *item = 0, int col = 0);
    void editeazaContract(int contract_id);

    void finalizeazaContract();
    void finalizeazaContract(int contract_id);

    void expandAll();
    void collapseAll();
};

#endif // MAINMDI_H
