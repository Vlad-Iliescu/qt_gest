#include "edit.h"
#include "ui_edit.h"

Edit::Edit(QWidget *parent) : QDialog(parent), ui(new Ui::Edit) {
    ui->setupUi(this);

    this->TVA = 0.24;
    this->menu = new QMenu(this);
    this->createActions();
    this->resetPrivileges();

    this->toDelete.clear();

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    ComandaDelegate *comDel = new ComandaDelegate(this);
    Oe_amDelegate *oe_amDel = new Oe_amDelegate(this);
    AsiguratorDelegate *asigDel = new AsiguratorDelegate(this->asiguratori,this);
    CantitateDelegate *cantDel = new CantitateDelegate(this);
    CantitateInDelegate *cantInDel = new CantitateInDelegate(this);
    FurnizorDelegate *furnDel = new FurnizorDelegate(QStringList(),this);
    TermenDelegate *termDel = new TermenDelegate(this);
    PretDelegate *pret_a = new PretDelegate(this);
    PretDelegate *pret_v = new PretDelegate(this);

    connect(pret_v,SIGNAL(pretChanged(double,int)),this,SLOT(calcPretVanzareTVA(double,int)));
    connect(cantDel,SIGNAL(cantitateChanged(int,int)),this,SLOT(calcTotal(int,int)));
    connect(comDel,SIGNAL(comandaChange()),this,SLOT(calcTotals()));
    connect(asigDel,SIGNAL(asiguratorChanged(int,int)),this,SLOT(changedAsig(int,int)));

    ui->tableWidget->setItemDelegateForColumn(0,comDel);
    ui->tableWidget->setItemDelegateForColumn(4,oe_amDel);
    ui->tableWidget->setItemDelegateForColumn(5,asigDel);
    ui->tableWidget->setItemDelegateForColumn(6,cantDel);
    ui->tableWidget->setItemDelegateForColumn(7,furnDel);
    ui->tableWidget->setItemDelegateForColumn(8,termDel);
    ui->tableWidget->setItemDelegateForColumn(11,pret_a);
    ui->tableWidget->setItemDelegateForColumn(12,pret_v);
    ui->tableWidget->setItemDelegateForColumn(15,cantInDel);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));

    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addNewRow()));
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(saveContract()));
    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(beforeClose()));
}

Edit::~Edit() {
    delete ui;
}

int Edit::getContractId() {
    return this->contract_id;
}

void Edit::createActions() {
    this->remove = new QAction(QIcon(":/remove"),"&Sterge",this);
    this->duplicate = new QAction(QIcon(":/duplicate"),"&Duplicare",this);
    this->add = new QAction(QIcon(":/add"),"&Adaugare",this);
    this->copy = new QAction(QIcon(":/copy"),"&Copy",this);
    this->total = new QAction(QIcon(":/calculator"),"&Total",this);
    this->comanda = new QAction(QIcon(":/next"),"C&omanda",this);

    this->add->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    connect(this->add,SIGNAL(triggered()),this,SLOT(addNewRow()));
    connect(this->copy,SIGNAL(triggered()),this,SLOT(copySelection()));
    connect(this->total,SIGNAL(triggered()),this,SLOT(totalSelection()));
    connect(this->comanda,SIGNAL(triggered()),this,SLOT(comandaMass()));

}

void Edit::duplicateRow(int row) {
    ui->tableWidget->insertRow(row+1);

    QTableWidgetItem *item, *newitem;
    for (int i=0;i<=6;++i) {
        if (i == 2) {
            continue;
        }
        item = ui->tableWidget->item(row,i);
        if (item) {
            newitem = new QTableWidgetItem();
            newitem->setText(item->text());
            newitem->setData(Qt::TextAlignmentRole,item->data(Qt::TextAlignmentRole));
            ui->tableWidget->setItem(row+1,i,newitem);
        }
    }

    for (int j = 0; j < ui->tableWidget->columnCount(); ++j) {
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row+1,j),this->privileges[j],Edit::ItemPrivilege);
    }

    ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row+1,2),0,Qt::UserRole);
    ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row+1,0),this->stareIcon(0),Qt::DecorationRole);

    item = new QTableWidgetItem();
    item->setData(Qt::UserRole,true);
    ui->tableWidget->setVerticalHeaderItem(row+1,item);

    this->colorRow(row+1);
}

void Edit::removeRow(int row) {
    QTableWidgetItem * item = ui->tableWidget->item(row,2);
    if (item) {
        bool ok;
        int linie_id = item->text().toInt(&ok);

        if (ok) {
            this->toDelete << linie_id;
        }
    }
    ui->tableWidget->removeRow(row);
}

void Edit::addNewRow() {
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);

    for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(ui->tableWidget->rowCount() - 1,i),this->privileges[i],Edit::ItemPrivilege);
    }

    ui->tableWidget->model()->setData(ui->tableWidget->model()->index(ui->tableWidget->rowCount() - 1,2),0,Qt::UserRole);
    ui->tableWidget->model()->setData(ui->tableWidget->model()->index(ui->tableWidget->rowCount() - 1,0),this->stareIcon(0),Qt::DecorationRole);

    QTableWidgetItem *item = new QTableWidgetItem();
    item->setData(Qt::UserRole,true);
    item->setText("   ");
    ui->tableWidget->setVerticalHeaderItem(ui->tableWidget->rowCount() - 1,item);

    this->colorRow(ui->tableWidget->rowCount() - 1);
}

void Edit::copySelection() {
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

void Edit::totalSelection() {
    QModelIndexList ranges = ui->tableWidget->selectionModel()->selectedIndexes();
    QList<int> rows;
    if (ranges.isEmpty()) {
        return;
    }

    foreach (const QModelIndex &index, ranges) {
        if (!rows.contains(index.row())) {
            rows.append(index.row());
        }
    }
//    qDebug() << rows;

    bool ok;
    QString text;
    double suma = 0.0;
    double term;

    foreach (int i, rows) {
        text = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i, 14)).toString();
        term = text.toDouble(&ok);
        if (ok) {
            suma += term;
        }
    }

    QMessageBox::information(this,"Total",QString("Totalul este: %1").arg(suma),QMessageBox::Ok);
}

void Edit::comandaMass() {
    QModelIndexList ranges = ui->tableWidget->selectionModel()->selectedIndexes();
    QList<int> rows;
    if (ranges.isEmpty()) {
        return;
    }

    foreach (const QModelIndex &index, ranges) {
        if (!rows.contains(index.row())) {
            rows.append(index.row());
        }
    }

    foreach(int i, rows) {
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,0),"Comanda",Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,0),true,Qt::UserRole);
    }
    this->calcTotals();
}

void Edit::calcPretVanzareTVA(double pret, int row) {
    double pretTVA = this->round(pret * ( 1 + TVA ));
    if (ui->tableWidget->item(row,13)) {
        ui->tableWidget->item(row,13)->setText(QString("%1").arg(pretTVA));
    } else {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setData(Qt::TextAlignmentRole,Qt::AlignRight);
        item->setText(QString("%1").arg(pretTVA));
        ui->tableWidget->setItem(row,13,item);
    }
    this->calcTotal(pret,row);
}

void Edit::calcTotal(double pret, int row) {
    if (!ui->tableWidget->item(row,6)) {
        return;
    }

    bool ok;
    int cantitate = ui->tableWidget->item(row,6)->text().toInt(&ok);
    if (!ok) {
        return;
    }

    if (ui->tableWidget->item(row,14)) {
        ui->tableWidget->item(row,14)->setText(QString("%1").arg(this->round((cantitate*pret)*(1 + TVA))));
    } else {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setData(Qt::TextAlignmentRole,Qt::AlignRight);
        item->setText(QString("%1").arg(this->round((cantitate*pret)*( 1 + TVA))));
        ui->tableWidget->setItem(row,14,item);
    }

    this->calcTotals();
}

void Edit:: calcTotal(int cantitate, int row) {
    if (!ui->tableWidget->item(row,12)) {
        return;
    }

    bool ok;
    double pret = ui->tableWidget->item(row,12)->text().toDouble(&ok);
    if (!ok) {
        return;
    }

    if (ui->tableWidget->item(row,14)) {
        ui->tableWidget->item(row,14)->setText(QString("%1").arg(this->round((cantitate*pret)*(1 + TVA))));
    } else {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setData(Qt::TextAlignmentRole,Qt::AlignRight);
        item->setText(QString("%1").arg(this->round((cantitate*pret)*(1 + TVA))));
        ui->tableWidget->setItem(row,14,item);
    }

    this->calcTotals();
}

void Edit::calcTotals() {

    double totalComanda = 0;
    double totalOferta = 0;
    double pret = 0;
    bool ok;
    bool hasText;
    bool isComanda;


    for (int i = 0; i< ui->tableWidget->rowCount();++i) {
        hasText = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,0),Qt::DisplayRole).toBool();
        if (hasText) {
            isComanda = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,0),Qt::UserRole).toBool();
            if (isComanda) {
                pret = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,14),Qt::DisplayRole).toDouble(&ok);
                if (ok) {
                    totalComanda += pret;
                }
            } else {
                pret = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,14),Qt::DisplayRole).toDouble(&ok);
                if (ok) {
                    totalOferta += pret;
                }
            }
        }
    }

    ui->totalComandaDoubleSpinBox->setValue(totalComanda);
    ui->totalOfertaDoubleSpinBox->setValue(totalOferta);
    ui->totalDoubleSpinBox->setValue(totalComanda + totalOferta);
}

void Edit::showContextMenu(const QPoint point) {
    this->menu->clear();
    QTableWidgetItem *selected = ui->tableWidget->itemAt(point);

    if (selected) {
        menu->addAction(this->duplicate);
        menu->addAction(this->remove);
        menu->addAction(this->copy);
        menu->addSeparator();
        menu->addAction(this->total);
        menu->addAction(this->comanda);
        menu->addSeparator();
    }

    this->menu->addAction(this->add);
    QAction *result = this->menu->exec(QCursor::pos());

    if (selected) {
        if (result == this->duplicate) {
            this->duplicateRow(selected->row());
        } else if (result == this->remove) {
            this->removeRow(selected->row());
        }
    }
}

void Edit::getDetaliiContract(int contract_id) {
    this->contract_id = contract_id;

    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());
    query.addQueryItem("contract_id",QString("%1").arg(contract_id));

    QUrl url = QUrl(QString(The::server_url() + "contracte/get_contract.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseContract(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Edit::parseContract(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Edit","parseContract();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Edit","parseContract();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        double TVA = result["TVA"].toDouble(&ok);
        if (ok) {
            this->TVA = this->round(TVA/100.0);
        } else {
            this->TVA = 0.24;
        }

        ui->userLineEdit->setText(result["user"].toString());
        ui->nrAutoLineEdit->setText(result["contract"].toMap()["numar_auto"].toString());
        ui->contractLineEdit->setText(result["contract"].toMap()["contract"].toString());
        ui->autoLineEdit->setText(result["contract"].toMap()["auto"].toString());
        if (!result["contract"].toMap()["programare"].toString().isEmpty()) {
            QDateTime prog = result["contract"].toMap()["programare"].toDateTime();
            ui->programareCheckBox->setChecked(true);
            ui->programareDateTimeEdit->setEnabled(true);
            ui->programareDateTimeEdit->setDateTime(prog);
        } else {
            ui->programareCheckBox->setChecked(false);
            ui->programareDateTimeEdit->setEnabled(false);
        }
        ui->contactatCheckBox->setChecked(result["contract"].toMap()["contactat"].toBool());
        ui->observatiiTextEdit->setPlainText(result["contract"].toMap()["observatii"].toString());
    }

    // Asiguratori
    this->asiguratori.clear();
    foreach(const QVariant &asig, result["asiguratori"].toList()) {
        this->asiguratori.insert(asig.toMap()["denumire"].toString(),asig.toMap()["tip"].toInt());
    }
    AsiguratorDelegate *asigDel = static_cast<AsiguratorDelegate*>(ui->tableWidget->itemDelegateForColumn(5));
    asigDel->setAsiguratori(this->asiguratori);
    // End Asiguratori

    // Linii
    int i = 0;
    int termen;
    int cantitate;
    int tip_asig = 0;
    double pret_v;
    double totalOferta = 0;
    double totalComanda = 0;
    QDateTime arrive;
    QString comanda;
    QString oe_am;
    QVariant temp;
    QTableWidgetItem *item;
    QString contract;

    contract = result["contract"].toMap()["contract"].toString();
    if (!contract.isEmpty()) {
        this->setWindowTitle(QString("Editare Contract - %1").arg(contract));
    }

    QDateTime now = (QVariant(result["data"]).canConvert(QVariant::DateTime))?(result["data"].toDateTime()):(QDateTime::currentDateTime());
//    qDebug() << "NOW" <<  now;
    ui->programareDateTimeEdit->setDateTime(now);

    ui->tableWidget->clearContents();
    this->nr_linii = result["linii"].toList().count();
    ui->tableWidget->setRowCount(this->nr_linii);

    foreach(const QVariant &linie, result["linii"].toList()) {
        // Header
        item = ui->tableWidget->verticalHeaderItem(i);
        if (!item) {
            item = new QTableWidgetItem();
            item->setText("   ");
            item->setData(Qt::UserRole,true);
            ui->tableWidget->setVerticalHeaderItem(i,item);
        }
        // Header

        // Comanda
        temp = linie.toMap()["comanda"];
        comanda = temp.toBool()?"Comanda":"Oferta";
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,0),comanda,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,0),temp,Qt::UserRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,0),this->privileges[0],Edit::ItemPrivilege);
        // End Comanda

        // Contract
        temp = result["contract"].toMap()["id"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,1),temp,Qt::DisplayRole);
        //End Contract

        //Id Linie
        temp = linie.toMap()["id"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,2),temp,Qt::DisplayRole);
        // End Id Linie

        // Stare
        temp = linie.toMap()["stare"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,2),temp.toBool()?temp:0,Qt::UserRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,0),this->stareIcon(temp.toInt()),Qt::DecorationRole);
        // End Stare

        // Denumire
        temp = linie.toMap()["denumire"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,3),temp,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,3),this->privileges[3],Edit::ItemPrivilege);
        // End Denumire

        // OE_AM
        oe_am = (linie.toMap()["oe_am"].toString().toLower() == "am")?"AM":"OE";
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,4),oe_am,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,4),Qt::AlignCenter,Qt::TextAlignmentRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,4),this->privileges[4],Edit::ItemPrivilege);
        // End OE_AM

        // Asigurator
        oe_am = (linie.toMap()["asigurator"].toString().isEmpty())?"--Fara--":linie.toMap()["asigurator"].toString();
        tip_asig = this->asiguratori.value(oe_am,0);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,5),oe_am,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,5),tip_asig,Qt::UserRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,5),this->privileges[5],Edit::ItemPrivilege);
        // End Asigurator

        // Cantitate
        temp = linie.toMap()["cantitate"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,6),temp,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,6),Qt::AlignCenter,Qt::TextAlignmentRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,6),this->privileges[6],Edit::ItemPrivilege);
        cantitate = temp.toInt();
        // End Cantitate

        // Furnizor
        temp = linie.toMap()["furnizor"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,7),temp,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,7),this->privileges[7],Edit::ItemPrivilege);
        // End Furnizor

        // Termen
        temp = linie.toMap()["termen"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,8),temp,Qt::UserRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,8),Qt::AlignRight,Qt::TextAlignmentRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,8),this->privileges[8],Edit::ItemPrivilege);

        termen = temp.toInt(&ok);
        if (ok) {
            arrive = (QVariant(linie.toMap()["data_edit"]).canConvert(QVariant::DateTime)?(linie.toMap()["data_edit"].toDateTime()):now);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,8),arrive,Qt::WhatsThisRole);

            while (termen) {
                arrive = arrive.addDays(1);
                if ((arrive.date().dayOfWeek() == Qt::Saturday) || (arrive.date().dayOfWeek() == Qt::Sunday)) {
                    arrive = arrive.addDays(1);
                } else {
                    --termen;
                }
            }

            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,8),arrive.toString("dd-MM-yyyy"),Qt::DisplayRole);
        } else {
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,8),now,Qt::WhatsThisRole);
        }
        // End Termen

        // Cod
        temp = linie.toMap()["cod"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,9),temp,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,9),this->privileges[9],Edit::ItemPrivilege);
        // End Cod

        // Cod Cross
        temp = linie.toMap()["cod_cross"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,10),temp,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,10),this->getPrivCol(tip_asig,10),Edit::ItemPrivilege);
        // End Cod Cross

        // Pret Achizitie
        temp = linie.toMap()["pret_achizitie"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,11),this->round(temp.toDouble()),Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,11),Qt::AlignRight,Qt::TextAlignmentRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,11),this->getPrivCol(tip_asig,11),Edit::ItemPrivilege);
        // End Pret Achizitie

        // Pret Vanzare
        temp = linie.toMap()["pret_vanzare"];
        pret_v = this->round(temp.toDouble());
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,12),pret_v,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,12),Qt::AlignRight,Qt::TextAlignmentRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,12),this->getPrivCol(tip_asig,12),Edit::ItemPrivilege);
        // End Pret Vanzare

        // Pret Vanzare TVA
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,13),
                                          this->round(pret_v *(1 +  this->round(TVA))),Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,13),Qt::AlignRight,Qt::TextAlignmentRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,13),this->privileges[13],Edit::ItemPrivilege);
        // End Pret Vanzare TVA

        // Total + TVA
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,14),
                                          this->round((pret_v * ( 1 + this->round(TVA))) * cantitate),Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,14),Qt::AlignRight,Qt::TextAlignmentRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,14),this->privileges[14],Edit::ItemPrivilege);
        // End Total + TVA

        if (comanda == "Comanda") {
            totalComanda += this->round((pret_v * ( 1 + this->round(TVA))) * cantitate);
        } else {
            totalOferta += this->round((pret_v * ( 1 + this->round(TVA))) * cantitate);
        }

        // Cantitate In
        temp = linie.toMap()["cantitate_in"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,15),temp,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,15),Qt::AlignCenter,Qt::TextAlignmentRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,15),this->privileges[15],Edit::ItemPrivilege);
        // End Cantitate In

        // Locatie
        temp = linie.toMap()["locatie"];
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,16),temp,Qt::DisplayRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,16),this->privileges[16],Edit::ItemPrivilege);
        // End locatie

        ++i;
    }

    ui->totalComandaDoubleSpinBox->setValue(totalComanda);
    ui->totalOfertaDoubleSpinBox->setValue(totalOferta);
    ui->totalDoubleSpinBox->setValue(totalComanda + totalOferta);
    // End Linii

    //Furnizori
    this->furnizori.clear();
    foreach(const QVariant &furn,result["furnizori"].toList()) {
        this->furnizori << furn.toMap()["denumire"].toString();
    }
    FurnizorDelegate *furnDel = static_cast<FurnizorDelegate*>(ui->tableWidget->itemDelegateForColumn(7));
    furnDel->setFurnizori(this->furnizori);
    // End Furnizori

    // Consilier
    this->consilieri.clear();
    ui->consilierComboBox->clear();

    foreach(const QVariant &cons, result["consilieri"].toList()) {
        this->consilieri.insert(cons.toMap()["nume"].toString(),cons.toMap()["id"].toInt());
        ui->consilierComboBox->addItem(cons.toMap()["nume"].toString(),cons.toMap()["id"]);
    }
    QVariant consilier = result["contract"].toMap()["consilier_id"];
    ui->consilierComboBox->setCurrentIndex(ui->consilierComboBox->findData(consilier,Qt::UserRole,Qt::MatchExactly));
    // End Consilier


    this->colorCells();
}

void Edit::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
    ui->saveButton->setEnabled(false);
    ui->cancelButton->setEnabled(false);
}

void Edit::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
    ui->saveButton->setEnabled(true);
    ui->cancelButton->setEnabled(true);
}

void Edit::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"Edit","handleNetworkError();",Logger::Network);
}

QVariant Edit::getCellData(int row, int col, Qt::ItemDataRole role, bool *empty) {
    *empty = ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,col),role).toString().isEmpty();

    return ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,col),role);
}

void Edit::saveContract() {
     if (QMessageBox::question(this,"Confirmare","Salvati Modificariele?",
                                     QMessageBox::Yes,QMessageBox::No) == QMessageBox::No) {
        return;
     }

    QVariantList update;
    QVariantList insert;

    QVariantMap linii;

    bool ok;
    bool up;
    bool empty;
    bool errorLine = false;

    QVariantMap row;
    QColor err = QColor(255,132,107);
    int priv = 0;
    int stare = 0;
    QVariant temp;
    QDateTime data_edit;
    QTableWidgetItem *item;

    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        row.clear();
        up = false;

        item = ui->tableWidget->verticalHeaderItem(i);
        if (!item->data(Qt::UserRole).toBool()) {
            this->colorRow(i);
            continue;
        }

        // --------------------  Comanda  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,0),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,0,Qt::UserRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,0),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,0);
                row.insert("comanda",temp);
            }
        } else if (priv == Edit::Optional) {
            row.insert("comanda",temp);
        }
        // --------------------  End Comanda  --------------------

        // --------------------  Contract ID  --------------------
        if (ui->tableWidget->item(i,1)) {
            row.insert("contract_id", this->contract_id);
        }
        // End Contract ID

        // Linie ID
        if (ui->tableWidget->item(i,2)) {
            row.insert("linie_id", ui->tableWidget->item(i,2)->text().toInt(&ok));
            if (ok) {
                up = true;
            }
        }
        // --------------------  End Linie ID  --------------------

        // --------------------  Stare  --------------------
        temp = getCellData(i,2,Qt::UserRole,&empty);
        stare = temp.toInt(&ok);
        if (!empty) {
            row.insert("stare",stare);
        }
        // --------------------  End Stare  --------------------


        // --------------------  Denumire  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,3),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,3,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,3),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,3);
                row.insert("denumire",QVariant(temp.toString().trimmed().toUpper()));
            }
        } else if (priv == Edit::Optional) {
            row.insert("denumire",QVariant(temp.toString().trimmed().toUpper()));
        }
        // --------------------  End Denumire  --------------------

        // --------------------  OE_AM  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,4),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,4,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,4),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,4);
                row.insert("oe_am",temp);
            }
        } else if (priv == Edit::Optional) {
            row.insert("oe_am",temp);
        }
        // --------------------  End OE_AM  --------------------

        // --------------------  Asigurator  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,5),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,5,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,5),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,5);
                row.insert("asigurator",temp);
            }
        } else if (priv == Edit::Optional) {
            row.insert("asigurator",temp);
        }
        // --------------------  End Asigurator  --------------------

        // -------------------- Cantitate  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,6),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,6,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,6),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,6);
                row.insert("cantitate",temp);
            }
        } else if (priv == Edit::Optional) {
            row.insert("cantitate",temp);
        }
        // --------------------  End Cantitate --------------------

        //-------------------- Furnizor --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,7),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,7,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,7),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,7);
                row.insert("furnizor",temp);
            }
        } else if (priv == Edit::Optional) {
            row.insert("furnizor",temp);
        }
        // --------------------  End Furnizor  --------------------

        // --------------------  Termen  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,8),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,8,Qt::UserRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,8),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,8);
                row.insert("termen",temp);

                // --------------------  Data Edit --------------------
                temp = getCellData(i,8,Qt::WhatsThisRole,new bool);
                data_edit = temp.canConvert(QVariant::DateTime)?temp.toDateTime():QDateTime::currentDateTime();
                row.insert("data_edit", data_edit.toString("dd.MM.yyyy HH:mm:ss"));
                // -------------------- End Data Edit --------------------
            }
        } else if (priv == Edit::Optional) {
            row.insert("termen",temp);

            // --------------------  Data Edit --------------------
            temp = getCellData(i,8,Qt::WhatsThisRole,new bool);
            data_edit = temp.canConvert(QVariant::DateTime)?temp.toDateTime():QDateTime::currentDateTime();
            row.insert("data_edit", data_edit.toString("dd.MM.yyyy HH:mm:ss"));
            // -------------------- End Data Edit --------------------
        }
        // --------------------  End Termen  --------------------

        // --------------------  Cod  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,9),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,9,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,9),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,9);
                row.insert("cod",QVariant(temp.toString().trimmed().toUpper()));
            }
        } else if (priv == Edit::Optional) {
            row.insert("cod",QVariant(temp.toString().trimmed().toUpper()));
        }
        // -------------------- End Cod --------------------

        // --------------------  Cod Cross  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,10),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,10,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,10),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,10);
                row.insert("cod_cross",QVariant(temp.toString().trimmed().toUpper()));
            }
        } else if (priv == Edit::Optional) {
            row.insert("cod_cross",QVariant(temp.toString().trimmed().toUpper()));
        }
        // --------------------  End Cod Cross  --------------------

        // -------------------- Pret Achizitie  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,11),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,11,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,11),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,11);
                row.insert("pret_achizitie",temp);
            }
        } else if (priv == Edit::Optional) {
            row.insert("pret_achizitie",temp);
        }
        // --------------------  End Pret Achizitie  --------------------

        // --------------------  Pret Vanzare  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,12),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,12,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,12),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,12);
                row.insert("pret_vanzare",temp);
            }
        } else if (priv == Edit::Optional) {
            row.insert("pret_vanzare",temp);
        }
        // --------------------  End Pret Vanzare --------------------

        // -------------------- Cantitate In  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,15),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,15,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,15),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,15);
                row.insert("cantitate_in",temp);
            }
        } else if (priv == Edit::Optional) {
            row.insert("cantitate_in",temp);
        }
        // --------------------  End Cantitate In  --------------------

        // --------------------  Locatie  --------------------
        priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,16),Edit::ItemPrivilege).toInt();
        temp = getCellData(i,16,Qt::DisplayRole,&empty);

        if (priv == Edit::Mandatory) {
            if (empty) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,16),err,Qt::BackgroundRole);
                errorLine = true;
            } else {
                this->colorCell(i,16);
                row.insert("locatie",QVariant(temp.toString().trimmed().toUpper()));
            }
        } else if (priv == Edit::Optional) {
            row.insert("locatie",QVariant(temp.toString().trimmed().toUpper()));
        }
        // --------------------  End Locatie  --------------------

        if (row.count()) {
            if (up) {
                update << row;
            } else {
                insert << row;
            }
        }
    }

    linii.insert("update", update);
    linii.insert("insert", insert);
    linii.insert("delete", this->toDelete);

    QByteArray json = The::serializer()->serialize(linii);

    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());
    query.addQueryItem("contract_id",QString("%1").arg(contract_id));

    QStringList errors;
    if (ui->contractLineEdit->text().isEmpty()) {
        errors << "Completati campul contract!";
    } else {
        query.addQueryItem("contract",ui->contractLineEdit->text().toUpper().trimmed());
    }

    if (ui->autoLineEdit->text().isEmpty()) {
        errors << "Completati campul Auto!";
    } else {
        query.addQueryItem("auto",ui->autoLineEdit->text().toUpper().trimmed());
    }

    if (ui->programareCheckBox->isChecked()) {
        query.addQueryItem("programare",ui->programareDateTimeEdit->dateTime().toString("dd.MM.yyyy HH:mm:ss"));
    }

    if (ui->contactatCheckBox->isChecked()) {
        query.addQueryItem("contactat","1");
    } else {
        query.addQueryItem("contactat","0");
    }

    if (!ui->observatiiTextEdit->toPlainText().isEmpty()){
        query.addQueryItem("obsevatii",ui->observatiiTextEdit->toPlainText().trimmed());
    }

    query.addQueryItem("consilier_id", ui->consilierComboBox->itemData(ui->consilierComboBox->currentIndex()).toString());
    query.addQueryItem("numar_auto",ui->nrAutoLineEdit->text().trimmed().toUpper());

//    qDebug() << ui->nrAutoLineEdit->text().trimmed().toUpper();

    if (errors.count() || errorLine) {
        QString message = errors.join("\n");
        if (errorLine) {
            if (message.length()) {
                message += "\n";
            }
            message += " Completati Celulele Rosii!";
        }
        QMessageBox::warning(this,"Aveti erori!", message);
        return;
    }

//    if (this->nr_linii && !(update.count() || insert.count()) && (this->toDelete.count() != this->nr_linii)) {
//        return;
//    }

    query.addQueryItem(QByteArray("linii"),json);

    QUrl url = QUrl(QString(The::server_url() + "contracte/edit_contract.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseSaveContract(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();

}

void Edit::parseSaveContract(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Edit","parseSaveContract();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Edit","parseSaveContract();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        this->reject();
    }
}

void Edit::changedAsig(int tip, int row) {
    ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,10),this->getPrivCol(tip,10),Qt::UserRole + 1);
    ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,11),this->getPrivCol(tip,11),Qt::UserRole + 1);
    ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,12),this->getPrivCol(tip,12),Qt::UserRole + 1);

    this->colorRow(row);
}

void Edit::resetPrivileges() {
    /*<< "Comanda" << "contract_id" << "id_linie" << "Denumire" << "OE/AM" << "Asigurator"
           << "Cant." << "Furnizor" << "Termen" << "Cod" << "Cod Cross" << "Pret Ach." << "Pret Vanz."
           << "Pret V. cu TVA" << "Total cu TVA" << "Cant. Venita" << "Locatie";

      */
    this->privileges.clear();
    this->privileges.insert(0,Edit::NotAllowed); // Comanda
    this->privileges.insert(1,Edit::NotAllowed); // contract_id
    this->privileges.insert(2,Edit::NotAllowed); // id_linie
    this->privileges.insert(3,Edit::NotAllowed); // Denumire
    this->privileges.insert(4,Edit::NotAllowed); // OE/AM
    this->privileges.insert(5,Edit::NotAllowed); // Asigurator
    this->privileges.insert(6,Edit::NotAllowed); // Cantitate
    this->privileges.insert(7,Edit::NotAllowed); // Furnizor
    this->privileges.insert(8,Edit::NotAllowed); // Termen
    this->privileges.insert(9,Edit::NotAllowed); // Cod
    this->privileges.insert(10,Edit::NotAllowed); // Cod_Cross
    this->privileges.insert(11,Edit::NotAllowed); // Pret_Ach
    this->privileges.insert(12,Edit::NotAllowed); // Pret_Vanz
    this->privileges.insert(13,Edit::NotAllowed); // Pret v. cu TVA
    this->privileges.insert(14,Edit::NotAllowed); // Total cu TVA
    this->privileges.insert(15,Edit::NotAllowed); // Cant. Venita
    this->privileges.insert(16,Edit::NotAllowed); // Locatie

    int nivel = The::nivel_user();

    ui->contractLineEdit->setEnabled(false);
    ui->autoLineEdit->setEnabled(false);
    ui->consilierComboBox->setEnabled(false);
    ui->programareCheckBox->setEnabled(false);
    ui->programareDateTimeEdit->setEnabled(false);
    ui->contactatCheckBox->setEnabled(false);

    if (nivel > 0) {
        ui->programareCheckBox->setEnabled(true);
        ui->programareDateTimeEdit->setEnabled(true);
        ui->contactatCheckBox->setEnabled(true);

        this->privileges[0] = Edit::Mandatory;
        this->privileges[3] = Edit::Mandatory;
        this->privileges[4] = Edit::Mandatory;
        this->privileges[5] = Edit::Mandatory;
        this->privileges[6] = Edit::Mandatory;
    }

    if (nivel > 1) {
        this->privileges[7] = Edit::Mandatory;
        this->privileges[8] = Edit::Mandatory;
        this->privileges[9] = Edit::Mandatory;
        this->privileges[10] = Edit::Optional;
        this->privileges[11] = Edit::Mandatory;
        this->privileges[12] = Edit::Mandatory;
    }

    if (nivel > 2) {
        ui->programareCheckBox->setEnabled(false);
        ui->programareDateTimeEdit->setEnabled(false);
        ui->contactatCheckBox->setEnabled(false);

        this->privileges[0] = Edit::NotAllowed;
        this->privileges[3] = Edit::NotAllowed;
        this->privileges[4] = Edit::NotAllowed;
        this->privileges[5] = Edit::NotAllowed;
        this->privileges[6] = Edit::NotAllowed;
        this->privileges[7] = Edit::NotAllowed;
        this->privileges[8] = Edit::NotAllowed;
        this->privileges[9] = Edit::NotAllowed;
        this->privileges[10] = Edit::NotAllowed;
        this->privileges[11] = Edit::NotAllowed;
        this->privileges[12] = Edit::NotAllowed;

        this->privileges[15] = Edit::Optional;
        this->privileges[16] = Edit::Optional;
    }

    if (nivel > 3) {
        ui->contractLineEdit->setEnabled(true);
        ui->autoLineEdit->setEnabled(true);
        ui->consilierComboBox->setEnabled(true);
        ui->programareCheckBox->setEnabled(true);
        ui->programareDateTimeEdit->setEnabled(true);
        ui->contactatCheckBox->setEnabled(true);

        this->privileges[0] = Edit::Optional;
        this->privileges[3] = Edit::Optional;
        this->privileges[4] = Edit::Optional;
        this->privileges[5] = Edit::Optional;
        this->privileges[6] = Edit::Optional;
        this->privileges[7] = Edit::Optional;
        this->privileges[8] = Edit::Optional;
        this->privileges[9] = Edit::Optional;
        this->privileges[10] = Edit::Optional;
        this->privileges[11] = Edit::Optional;
        this->privileges[12] = Edit::Optional;
        this->privileges[15] = Edit::Optional;
        this->privileges[16] = Edit::Optional;
    }
}

void Edit::colorCells() {
    QColor mandatory = QColor(250, 250, 210);
    QColor notAllowed = QColor(240, 240, 240);
    QColor optional = QColor(255, 255, 255);
    QTableWidgetItem *item;

    for(int i = 0; i<ui->tableWidget->rowCount();++i) {
        for (int j = 0; j<ui->tableWidget->columnCount(); ++j) {
            QAbstractItemModel *m = ui->tableWidget->model();
            QModelIndex in = ui->tableWidget->model()->index(i,j);
            int priv = m->data(in, Edit::ItemPrivilege).toInt();

            item = ui->tableWidget->item(i,j);
            if (item) {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            } else {
                item = new QTableWidgetItem();
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,j,item);
            }

            if (priv == Edit::Optional) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,j),optional,Qt::BackgroundRole);
            } else if (priv == Edit::Mandatory) {
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,j),mandatory,Qt::BackgroundRole);
            } else if (priv == Edit::NotAllowed) {
                if (item) {
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                }
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i,j),notAllowed,Qt::BackgroundRole);
            }
        }
    }
}

void Edit::colorRow(int row) {
    QColor mandatory = QColor(250, 250, 210);
    QColor notAllowed = QColor(240, 240, 240);
    QColor optional = QColor(255, 255, 255);
    QTableWidgetItem *item;


    for (int j = 0; j<ui->tableWidget->columnCount(); ++j) {

        item = ui->tableWidget->item(row,j);
        if (item) {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        } else {
            item = new QTableWidgetItem();
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->tableWidget->setItem(row,j,item);
        }

        int priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,j),Edit::ItemPrivilege).toInt();

        if (priv == Edit::Optional) {
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,j),optional,Qt::BackgroundRole);
        } else if (priv == Edit::Mandatory) {
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,j),mandatory,Qt::BackgroundRole);
        } else if (priv == Edit::NotAllowed) {
            if (item) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,j),notAllowed,Qt::BackgroundRole);
        }
    }
}

void Edit::colorCell(int row, int col) {
    QColor mandatory = QColor(250, 250, 210);
    QColor notAllowed = QColor(240, 240, 240);
    QColor optional = QColor(255, 255, 255);
    QTableWidgetItem *item;

    item = ui->tableWidget->item(row,col);
    if (item) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    } else {
        item = new QTableWidgetItem();
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->tableWidget->setItem(row,col,item);
    }

    int priv = ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,col),Edit::ItemPrivilege).toInt();

    if (priv == Edit::Optional) {
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,col),optional,Qt::BackgroundRole);
    } else if (priv == Edit::Mandatory) {
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,col),mandatory,Qt::BackgroundRole);
    } else if (priv == Edit::NotAllowed) {
        if (item) {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(row,col),notAllowed,Qt::BackgroundRole);
    }
}

QPixmap Edit::stareIcon(int stare) {
    QPixmap color = QPixmap(10,10);
    if (stare == 0) {
        color.fill(Qt::gray);
    } else if (stare == 1) {
        color.fill(Qt::darkMagenta);
    } else if (stare == 2) {
        color.fill(Qt::cyan);
    } else if (stare == 3){
        color.fill(Qt::darkCyan);
    } else if (stare == 4) {
        color.fill(Qt::red);
    } else if (stare == 5) {
        color.fill(Qt::green);
    }
    return color;
}

void Edit::beforeClose() {
    if (QMessageBox::question(this,"Confirmare","Modificarile nu vor fi salvate, iesiti?",
                                    QMessageBox::Yes,QMessageBox::No) == QMessageBox::No) {
       return;
    }
    this->close();
}

Edit::Privilege Edit::getPrivCol(int tip, int col) {
    if (The::nivel_user() == 2 || The::nivel_user() > 3) {
        if (col == 10 || col == 11 || col == 12) {
            if (tip == 2) {
                return Edit::Optional;
            }
        }
    }

    return this->privileges[col];
}
