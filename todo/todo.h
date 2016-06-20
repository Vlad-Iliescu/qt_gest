#ifndef TODO_H
#define TODO_H

#include <QDialog>
#include <QTableWidgetItem>

#include "global.h"
#include "log.h"

namespace Ui {
    class ToDo;
}

class ToDo : public QDialog
{
    Q_OBJECT

signals:
    void editContract(int contract_id);
    void doneParsing();
    void kill();

public:
    explicit ToDo(QWidget *parent = 0);
    ~ToDo();

public slots:
    void parsePopup(QVariantMap data);

private:
    Ui::ToDo *ui;

private slots:
    void itemDC(QTableWidgetItem *item = 0);
    void colorRow(int row);

};

#endif // TODO_H
