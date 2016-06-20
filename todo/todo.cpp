#include "todo.h"
#include "ui_todo.h"

ToDo::ToDo(QWidget *parent) : QDialog(parent), ui(new Ui::ToDo) {
    ui->setupUi(this);

    ui->tableWidget->verticalHeader()->setDefaultSectionSize(18);
    ui->tableWidget->setColumnHidden(1,true);

    connect(ui->tableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(itemDC(QTableWidgetItem*)));
}

ToDo::~ToDo() {
    delete ui;
}

void ToDo::itemDC(QTableWidgetItem *item) {
    if (!item) {
        item = ui->tableWidget->currentItem();
    }
    bool ok;
    int contract_id = ui->tableWidget->item(item->row(),1)->text().toInt(&ok);

    if (ok) {
        emit editContract(contract_id);
    }
    this->done(1);
}

void ToDo::parsePopup(QVariantMap data) {
    if (this->isVisible()) {
        this->close();
    }
    int rows = data["pop"].toList().count();

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(rows);
    ui->tableWidget->setUpdatesEnabled(false);

    int i =0;
    foreach(const QVariant &con,data["pop"].toList()) {
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,0),con.toMap()["contract"],Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,1),con.toMap()["id"],Qt::DisplayRole);

        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,2),con.toMap()["auto"],Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,2),Qt::AlignLeft,Qt::TextAlignmentRole);

        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,3),con.toMap()["comenzi"],Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,3),Qt::AlignCenter,Qt::TextAlignmentRole);

        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,4),con.toMap()["oferte"],Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,4),Qt::AlignCenter,Qt::TextAlignmentRole);

        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,5),con.toMap()["receptionate"],Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,5),Qt::AlignCenter,Qt::TextAlignmentRole);

        if (con.toMap()["comenzi"].toBool()  && con.toMap()["comenzi"].toInt() == con.toMap()["receptionate"].toInt()) {
            this->colorRow(i);
        }

        ++i;
    }
    ui->tableWidget->setUpdatesEnabled(true);
    this->show();
}

void ToDo::colorRow(int row) {
    for (int i=0;i<ui->tableWidget->columnCount();++i) {
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,i),QBrush(QColor(210,250,211)),Qt::BackgroundRole);
    }
}
