#ifndef FINALIZATE_H
#define FINALIZATE_H

#include <QWidget>
#include <QMenu>
#include <QHeaderView>

#include "math.h"
#include "global.h"
#include "log.h"


namespace Ui {
    class Finalizate;
}

class Finalizate : public QWidget
{
    Q_OBJECT

signals:
    void loadingStart();
    void loadingStop();
    void tva(double);
    void kill();
    void editContract(int contract_id);

public:
    explicit Finalizate(QWidget *parent = 0);
    ~Finalizate();

public slots:
    void getContent();

private:
    Ui::Finalizate *ui;
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

    void editeazaContract(QTreeWidgetItem *item = 0, int col = 0);
    void editeazaContract(int contract_id);


};

#endif // FINALIZATE_H
