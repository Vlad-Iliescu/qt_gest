#ifndef CONTRACTEDELEGATE_H
#define CONTRACTEDELEGATE_H

#include <QTableWidget>
#include <QHeaderView>

#include "edit/table_model/customverticalheader.h"

class ContracteDelegate : public QTableWidget
{
    Q_OBJECT
public:
    explicit ContracteDelegate(QWidget *parent = 0);

signals:

private:
    CustomVerticalHeader *header;

};

#endif // CONTRACTEDELEGATE_H
