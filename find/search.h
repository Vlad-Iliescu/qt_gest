#ifndef SEARCH_H
#define SEARCH_H

#include <QDialog>
#include <QMovie>
#include <QNetworkReply>

#include "global.h"
#include "log.h"

namespace Ui {
    class Search;
}

class Search : public QDialog
{
    Q_OBJECT

signals:
    void kill();
    void editContract(int contract_id);
    void finalizareContract(int contract_id);

public:
    explicit Search(QWidget *parent = 0);
    ~Search();

public slots:
    void resetPrivileges();

private:
    Ui::Search *ui;
    QMovie *movie;
    QAction *finalizare;
    QMenu *menu;

private slots:
    void loadingStart();
    void loadingStop();

    void search();
    void parseAnswer(QByteArray data);

    void getFurnizori();
    void parseFurnizori(QByteArray data);

    void itemDoubleClicked(QTreeWidgetItem *item, int col);

    void handleNetworkError(QNetworkReply *reply);

    void showContextMenu(const QPoint point);

};

#endif // SEARCH_H
