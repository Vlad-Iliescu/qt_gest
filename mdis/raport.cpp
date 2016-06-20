#include "raport.h"
#include "ui_raport.h"

Raport::Raport(QWidget *parent) : QDialog(parent), ui(new Ui::Raport) {
    ui->setupUi(this);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(18);
    ui->tableWidget->horizontalHeader()->setSectionsMovable(true);
    ui->tableWidget->setSortingEnabled(true);

    connect(ui->copyButton,SIGNAL(clicked()),this,SLOT(copySelection()));
}

Raport::~Raport() {
    delete ui;
}

void Raport::populate(QVariantMap map) {
    ui->tableWidget->clear();

    this->setWindowTitle(QString("Raport - %1").arg(map["denumire"].toString()));

    QStringList headers = map["cols"].toStringList();
    ui->tableWidget->setColumnCount(headers.count());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setRowCount(map["raport"].toList().count());

    QTableWidgetItem *item;
    int i;
    int j;

    i = 0;
    foreach(const QVariant &raport, map["raport"].toList()) {
        j = 0;
        foreach(const QString &col,map["cols"].toStringList()) {
            item = new QTableWidgetItem();
            item->setText(raport.toMap()[col].toString());
            ui->tableWidget->setItem(i,j,item);
            ++j;
        }
        ++i;
    }
}

void Raport::copySelection() {
    QString text;

    QList<QTableWidgetSelectionRange> ranges = ui->tableWidget->selectedRanges();
    QTableWidgetSelectionRange range;
    if (ranges.isEmpty()) {
        return;
    }
    range = ranges.first();

    QStringList list;
    for (int i = 0; i< range.columnCount(); ++i) {
        list << ui->tableWidget->horizontalHeaderItem(range.leftColumn() + i)->text();
    }

    text += list.join("\t");
    text += "\n";

    for (int i = 0; i < range.rowCount(); ++i) {
        if (i > 0) {
            text += "\n";
        }
        for (int j = 0; j < range.columnCount(); ++j) {
            if (j > 0) {
                text += "\t";
            }
            text += ui->tableWidget->model()->data(ui->tableWidget->model()->index(range.topRow() + i, range.leftColumn() + j)).toString();
        }
    }

    qApp->clipboard()->setText(text);
}
