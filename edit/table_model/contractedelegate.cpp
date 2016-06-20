#include "contractedelegate.h"

ContracteDelegate::ContracteDelegate(QWidget *parent) : QTableWidget(parent) {

    this->setColumnCount(17);
    QStringList labels;
    labels << "Comanda" << "contract_id" << "id_linie" << "Denumire" << "OE/AM" << "Asigurator"
           << "Cant." << "Furnizor" << "Termen" << "Cod" << "Cod Cross" << "Pret Ach." << "Pret Vanz."
           << "Pret V. cu TVA" << "Total cu TVA" << "Cant. Venita" << "Locatie";

    this->setHorizontalHeaderLabels(labels);
//    this->horizontalHeader()->setResizeMode(3,QHeaderView::Stretch);

    this->setSelectionBehavior(QTableWidget::SelectRows);
    this->setDragEnabled(false);
    this->setSortingEnabled(true);

    this->header = new CustomVerticalHeader(this);
    this->setVerticalHeader(this->header);

    this->setColumnWidth(0,75);
    this->setColumnWidth(4,48);
    this->setColumnWidth(5,96);
    this->setColumnWidth(6,40);
    this->setColumnWidth(7,103);
    this->setColumnWidth(8,67);
    this->setColumnWidth(11,60);
    this->setColumnWidth(12,60);
    this->setColumnWidth(13,60);
    this->setColumnWidth(14,60);
    this->setColumnWidth(15,60);
    this->setColumnWidth(16,60);

    this->setColumnHidden(1,true);
    this->setColumnHidden(2,true);
}

