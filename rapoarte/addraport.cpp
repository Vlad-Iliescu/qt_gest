#include "addraport.h"
#include "ui_addraport.h"

AddRaport::AddRaport(QWidget *parent) : QDialog(parent), ui(new Ui::AddRaport) {
    ui->setupUi(this);
    this->visible = true;

    this->resetPrivileges();

    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->date_add_maxDateTimeEdit->setDate(QDate::currentDate());
    ui->date_add_minDateTimeEdit->setDate(QDate::currentDate());
    ui->programare_intre_minDateTimeEdit->setDate(QDate::currentDate());
    ui->programare_intre_maxDateTimeEdit->setDate(QDate::currentDate());
    ui->date_edit_minDateTimeEdit->setDate(QDate::currentDate());
    ui->date_edit_maxDateTimeEdit->setDate(QDate::currentDate());

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    this->raport_id = 0;


    this->cols["Contract"] = "contracte.contract";
    this->cols["Auto"] = "contracte.auto";
    this->cols["Numar Auto"] = "contracte.numar_auto";
    this->cols["Data Adaugare Contract"] = "contracte.date_added";
    this->cols["User Adaugare Contract"] = "contracte.user_id";
    this->cols["Programare"] = "contracte.programare";
    this->cols["Contactat"] = "contracte.contactat";
    this->cols["Consilier Contract"] = "contracte.consilier_id";
    this->cols["Finalizat"] = "contracte.finalizat";
    this->cols["Observatii"] = "contracte.observatii";
    this->cols["Motiv"] = "contracte.motiv";

    this->cols["User Adaugare Linie"] = "linii.user_id";
    this->cols["Data Adaugare Linie"] = "linii.date_added";
    this->cols["Comanda"] = "linii.comanda";
    this->cols["Denumire"] = "linii.denumire";
    this->cols["OE AM"] = "linii.oe_am";
    this->cols["Asigurator"] = "linii.asigurator";
    this->cols["Cantitate"] = "linii.cantitate";
    this->cols["Consilier Lucrat"] = "linii.consilier_id";
    this->cols["Data Completare"] = "linii.data_edit";
    this->cols["Furnizor"] = "linii.furnizor";
    this->cols["Termen"] = "linii.termen";
    this->cols["Cod"] = "linii.cod";
    this->cols["Cod Cross"] = "linii.cod_cross";
    this->cols["Pret Achizitie"] = "linii.pret_achizitie";
    this->cols["Pret Vanzare"] = "linii.pret_vanzare";
    this->cols["Cantitate Receptie"] = "linii.cantitate_in";
    this->cols["User Receptie"] = "linii.user_receptie";
    this->cols["Data Receptie"] = "linii.data_receptie";
    this->cols["Stare"] = "linii.stare";
    this->cols["Sters"] = "linii.sters";
    this->cols["Locatie"] = "linii.locatie";

    QListWidgetItem *item, *item2;
    for (QMap<QString, QString>::const_iterator i = this->cols.constBegin(); i!= this->cols.constEnd();++i) {
        item = new QListWidgetItem(ui->coloaneListWidget);
        item->setText(i.key());
        item->setCheckState(Qt::Checked);

        item2 = new QListWidgetItem(ui->active_coloaneListWidget);
        item2->setText(i.key());
        item2->setCheckState(Qt::Unchecked);
    }

    connect(ui->viewButton,SIGNAL(clicked()),this,SLOT(viewRaport()));
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(saveRaport()));
    connect(ui->checkAllButton,SIGNAL(clicked()),this,SLOT(checkAll()));
    connect(ui->uncheckAllButton,SIGNAL(clicked()),this,SLOT(uncheckAll()));
    connect(ui->active_checkAllButton,SIGNAL(clicked()),this,SLOT(activeCheckAll()));
    connect(ui->active_uncheckAllButton,SIGNAL(clicked()),this,SLOT(activeUncheckAll()));

    connect(this,SIGNAL(rejected()),this,SLOT(close()));

    connect(ui->active_coloaneListWidget,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(itemCheckChenged(QListWidgetItem*)));

    AQP::accelerateTabWidget(ui->tabWidget);

    this->createConnections();

    this->getting_data = true;
    this->getData();

    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    while (this->getting_data) {
        qApp->processEvents();
    }
    qApp->restoreOverrideCursor();
}

AddRaport::~AddRaport() {
//    qDebug() << "delete";
    delete ui;
}

void AddRaport::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
}

void AddRaport::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
}

void AddRaport::checkAll() {
    for (int i = 0; i<ui->coloaneListWidget->count();++i) {
        ui->coloaneListWidget->item(i)->setCheckState(Qt::Checked);
    }
}

void AddRaport::uncheckAll() {
    for (int i = 0; i<ui->coloaneListWidget->count();++i) {
        ui->coloaneListWidget->item(i)->setCheckState(Qt::Unchecked);
    }

}

void AddRaport::activeCheckAll() {
    QList<QCheckBox*> widgets = ui->active->findChildren<QCheckBox*>();

    foreach (QCheckBox *w, widgets) {
        w->setChecked(true);
    }
}

void AddRaport::activeUncheckAll() {
    QList<QCheckBox*> widgets = ui->active->findChildren<QCheckBox*>();

    foreach (QCheckBox *w, widgets) {
        w->setChecked(false);
    }
}

void AddRaport::itemCheckChenged(QListWidgetItem *item) {
    Qt::CheckState check = item->checkState();
    QList<QListWidgetItem*> items = ui->coloaneListWidget->findItems(item->text(),Qt::MatchCaseSensitive);

    if (check == Qt::Checked) {
        if (items.count())  {
            items.at(0)->setHidden(true);
        }
    } else if (check == Qt::Unchecked) {
        if (items.count())  {
            items.at(0)->setHidden(false);
        }
    }
}

QVariantList AddRaport::getColumns() {
    QVariantList list;
    QVariantMap line;
    for(int i = 0; i<ui->coloaneListWidget->count();++i) {
        if (ui->coloaneListWidget->item(i)->checkState() == Qt::Checked) {
            line.clear();
            line.insert("column",this->cols.value(ui->coloaneListWidget->item(i)->text(),"None"));
            line.insert("alias", ui->coloaneListWidget->item(i)->text());
            list << line;
        }
    }
    return list;
}

QVariantList AddRaport::getHiddenColumns() {
    QVariantList list;
    QVariantMap line;
    for (int i = 0; i<ui->active_coloaneListWidget->count();++i) {
        if (ui->active_coloaneListWidget->item(i)->checkState() == Qt::Checked) {
            line.clear();
            line.insert("column",this->cols.value(ui->active_coloaneListWidget->item(i)->text(),"None"));
            list << line;
        }
    }
    return list;
}

void AddRaport::getData() {
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());

    QUrl url = QUrl(QString(The::server_url() + "rapoarte/get_data_raport.json"));
    QObject *scan = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(scan, SIGNAL(data(QByteArray)),this, SLOT(parseData(QByteArray)));
    connect(scan, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void AddRaport::parseData(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","AddRaport","parseData();",Logger::JSON);
        this->getting_data = false;
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"AddRaport","parseData();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
    } else {
        foreach(const QVariant &user, result["users"].toList()) {
            this->users[user.toMap()["id"].toInt()] = user.toMap()["nume"].toString();
            if (user.toMap()["nivel"].toInt() != 3){
                ui->user_add_contractComboBox->addItem(user.toMap()["nume"].toString(),true);
                ui->user_add_linieComboBox->addItem(user.toMap()["nume"].toString(),true);
            }
            if (user.toMap()["nivel"].toInt() == 3 || user.toMap()["nivel"].toInt() == 4) {
               ui->user_magazieComboBox->addItem(user.toMap()["nume"].toString(),true);
            }
        }
        ui->user_add_contractComboBox->uncheckAll();
        ui->user_add_linieComboBox->uncheckAll();
        foreach(const QVariant &co, result["consilieri"].toList()) {
            this->cons[co.toMap()["id"].toInt()] = co.toMap()["nume"].toString();
            ui->consilier_contractComboBox->addItem(co.toMap()["nume"].toString(),true);
            if (co.toMap()["id"].toInt() != 14) {
                ui->consilier_lucratComboBox->addItem(co.toMap()["nume"].toString(),true);
            }
        }
        ui->consilier_contractComboBox->uncheckAll();
        ui->consilier_lucratComboBox->uncheckAll();

        ui->asiguratorComboBox->addItem("--FARA--",true);
        foreach(const QVariant &as, result["asigurator"].toList()) {
            ui->asiguratorComboBox->addItem(as.toMap()["denumire"].toString(),true);
        }
        foreach(const QVariant &fu, result["furnizori"].toList()) {
            ui->furnizorComboBox->addItem(fu.toMap()["denumire"].toString(),true);
        }
        ui->asiguratorComboBox->uncheckAll();
        ui->furnizorComboBox->uncheckAll();
    }
    this->getting_data = false;
}

void AddRaport::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"AddRaport","handleNetworkError();",Logger::Network);
}

void AddRaport::saveRaport() {
    QUrlQuery query;
    QVariantMap json;

    if (ui->numeEdit->text().isEmpty()) {
        QMessageBox::warning(this,"Eroare","Numele nu este corect!");
        return;
    } else {
        query.addQueryItem("nume", ui->numeEdit->text());
    }

    if (this->raport_id) {
        query.addQueryItem("raport_id", QString("%1").arg(this->raport_id));

    }

    json.insert("colums",this->getColumns());
    json.insert("disabled",this->getDiabledWidgets());
    json.insert("active",this->getActiveWidgets());
    json.insert("hidden_colums",this->getHiddenColumns());

    query.addQueryItem("user_hash",The::user_hash());
    query.addQueryItem("data",The::serializer()->serialize(json));
    query.addQueryItem("tot_contractul",ui->totContractulCheckBox->isChecked()?"1":"0");

    QUrl url;

    if (this->raport_id) {
        url = QUrl(QString(The::server_url() + "rapoarte/editeaza_raport.json"));;
    } else {
        url = QUrl(QString(The::server_url() + "rapoarte/add_raport.json"));
    }

    QObject *scan = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8().replace("+","%2B"));

    connect(scan, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(scan, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void AddRaport::parseAnswer(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","AddRaport","parseData();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"AddRaport","parseData();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        emit rapoarteChanged();
        this->close();
    }
}

void AddRaport::createConnections() {
    if (this->visible) {
        // nume
        connect(ui->active_numeCheckBox,SIGNAL(toggled(bool)),ui->numeEdit,SLOT(setDisabled(bool)));
        // contract
        connect(ui->active_contractCheckBox,SIGNAL(toggled(bool)),ui->contractWidget,SLOT(setDisabled(bool)));
        // auto
        connect(ui->active_autoCheckBox,SIGNAL(toggled(bool)),ui->autoWidget,SLOT(setDisabled(bool)));
        // denumire
        connect(ui->active_denumireCheckBox,SIGNAL(toggled(bool)),ui->denumireWidget,SLOT(setDisabled(bool)));
        // oe-am
        connect(ui->active_OE_AMcheckBox,SIGNAL(toggled(bool)),ui->oe_amWidget,SLOT(setDisabled(bool)));
        // consilier contract
        connect(ui->active_consilier_contractCheckBox,SIGNAL(toggled(bool)),ui->consilierContractWidget,SLOT(setDisabled(bool)));
        //consilier lucrat
        connect(ui->active_consilier_lucratCheckBox,SIGNAL(toggled(bool)),ui->consilierLucratWidget,SLOT(setDisabled(bool)));
        // asigurator
        connect(ui->active_asiguratorCheckBox,SIGNAL(toggled(bool)),ui->asiguratorWidget,SLOT(setDisabled(bool)));
        // furnizor
        connect(ui->active_furnizorCheckBox,SIGNAL(toggled(bool)),ui->furnizorWidget,SLOT(setDisabled(bool)));
        // cod
        connect(ui->active_codCheckBox,SIGNAL(toggled(bool)),ui->codWidget,SLOT(setDisabled(bool)));
        // cod cross
        connect(ui->active_cod_crossCheckBox,SIGNAL(toggled(bool)),ui->codCrossWidget,SLOT(setDisabled(bool)));
        // user adaugare contract
        connect(ui->active_user_add_contractCheckBox,SIGNAL(toggled(bool)),ui->userAdaugareContractWidget,SLOT(setDisabled(bool)));
        // user adaugare linie
        connect(ui->active_user_add_linieCheckBox,SIGNAL(toggled(bool)),ui->userAdaugareLinieWidget,SLOT(setDisabled(bool)));
        // user magazie
        connect(ui->active_user_magazieCheckBox,SIGNAL(toggled(bool)),ui->userMagazieWidget,SLOT(setDisabled(bool)));
        // locatie
        connect(ui->active_locatieCheckBox,SIGNAL(toggled(bool)),ui->locatieWidget,SLOT(setDisabled(bool)));
        // oferta-comanda
        connect(ui->active_oferta_comandaCheckBox,SIGNAL(toggled(bool)),ui->ofertaComandaWidget,SLOT(setDisabled(bool)));
        // finalizat
        connect(ui->active_finalizatCheckBox,SIGNAL(toggled(bool)),ui->finalizatWidget,SLOT(setDisabled(bool)));
        // observatii
        connect(ui->activeObservatiiCheckBox,SIGNAL(toggled(bool)),ui->observatiiWidget,SLOT(setDisabled(bool)));
        //motiv
        connect(ui->active_motivCheckBox,SIGNAL(toggled(bool)),ui->motivCheckBox,SLOT(setDisabled(bool)));
        // stare
        connect(ui->active_stareCheckBox,SIGNAL(toggled(bool)),ui->stareGroupBox,SLOT(setDisabled(bool)));
        // data adaugare contract
        connect(ui->active_data_add_contractCheckBox,SIGNAL(toggled(bool)),ui->dataAdaugareContractGroupBox,SLOT(setDisabled(bool)));
        // data adaugare linie
        connect(ui->active_data_add_linieCheckBox,SIGNAL(toggled(bool)),ui->dataAdaugareLinieGroupBox,SLOT(setDisabled(bool)));
        // data completare
        connect(ui->active_data_completareCheckBox,SIGNAL(toggled(bool)),ui->dataAdaugareCompletareGroupBox,SLOT(setDisabled(bool)));
        // data receptie
        connect(ui->active_data_receptieCheckBox,SIGNAL(toggled(bool)),ui->dataReceptieGroupBox,SLOT(setDisabled(bool)));
        // cantitate
        connect(ui->active_cantitateCheckBox,SIGNAL(toggled(bool)),ui->cantitateGroupBox,SLOT(setDisabled(bool)));
        // termen
        connect(ui->active_termenCheckBox,SIGNAL(toggled(bool)),ui->termenGroupBox,SLOT(setDisabled(bool)));
        // cantitate receptionata
        connect(ui->active_cantitate_receptieCheckBox,SIGNAL(toggled(bool)),ui->cantitateReceptieGroupBox,SLOT(setDisabled(bool)));
        // pret achizitie
        connect(ui->active_pret_aCheckBox,SIGNAL(toggled(bool)),ui->pretAchGroupBox,SLOT(setDisabled(bool)));
        // pret vanzare
        connect(ui->active_pret_vCheckBox,SIGNAL(toggled(bool)),ui->pretVanzGroupBox,SLOT(setDisabled(bool)));
        // programare
        connect(ui->active_programareCheckBox,SIGNAL(toggled(bool)),ui->programareGroupBox,SLOT(setDisabled(bool)));
        // contactat
        connect(ui->active_contactatCheckBox,SIGNAL(toggled(bool)),ui->contactatGroupBox,SLOT(setDisabled(bool)));
    } else {
        // nume
        connect(ui->active_numeCheckBox,SIGNAL(toggled(bool)),ui->numeEdit,SLOT(setHidden(bool)));
        // contract
        connect(ui->active_contractCheckBox,SIGNAL(toggled(bool)),ui->contractWidget,SLOT(setHidden(bool)));
        // auto
        connect(ui->active_autoCheckBox,SIGNAL(toggled(bool)),ui->autoWidget,SLOT(setHidden(bool)));
        //denumire
        connect(ui->active_denumireCheckBox,SIGNAL(toggled(bool)),ui->denumireWidget,SLOT(setHidden(bool)));
        // oe-am
        connect(ui->active_OE_AMcheckBox,SIGNAL(toggled(bool)),ui->oe_amWidget,SLOT(setHidden(bool)));
        // consilier contract
        connect(ui->active_consilier_contractCheckBox,SIGNAL(toggled(bool)),ui->consilierContractWidget,SLOT(setHidden(bool)));
        //consilier lucrat
        connect(ui->active_consilier_lucratCheckBox,SIGNAL(toggled(bool)),ui->consilierLucratWidget,SLOT(setHidden(bool)));
        // asigurator
        connect(ui->active_asiguratorCheckBox,SIGNAL(toggled(bool)),ui->asiguratorWidget,SLOT(setHidden(bool)));
        // furnizor
        connect(ui->active_furnizorCheckBox,SIGNAL(toggled(bool)),ui->furnizorWidget,SLOT(setHidden(bool)));
        // cod
        connect(ui->active_codCheckBox,SIGNAL(toggled(bool)),ui->codWidget,SLOT(setHidden(bool)));
        // cod cross
        connect(ui->active_cod_crossCheckBox,SIGNAL(toggled(bool)),ui->codCrossWidget,SLOT(setHidden(bool)));
        // user adaugare contract
        connect(ui->active_user_add_contractCheckBox,SIGNAL(toggled(bool)),ui->userAdaugareContractWidget,SLOT(setHidden(bool)));
        // user adaugare linie
        connect(ui->active_user_add_linieCheckBox,SIGNAL(toggled(bool)),ui->userAdaugareLinieWidget,SLOT(setHidden(bool)));
        // user magazie
        connect(ui->active_user_magazieCheckBox,SIGNAL(toggled(bool)),ui->userMagazieWidget,SLOT(setHidden(bool)));
        // locatie
        connect(ui->active_locatieCheckBox,SIGNAL(toggled(bool)),ui->locatieWidget,SLOT(setHidden(bool)));
        // oferta-comanda
        connect(ui->active_oferta_comandaCheckBox,SIGNAL(toggled(bool)),ui->ofertaComandaWidget,SLOT(setHidden(bool)));
        // finalizat
        connect(ui->active_finalizatCheckBox,SIGNAL(toggled(bool)),ui->finalizatWidget,SLOT(setHidden(bool)));
        // observatii
        connect(ui->activeObservatiiCheckBox,SIGNAL(toggled(bool)),ui->observatiiWidget,SLOT(setHidden(bool)));
        //motiv
        connect(ui->active_motivCheckBox,SIGNAL(toggled(bool)),ui->motivCheckBox,SLOT(setHidden(bool)));
        // stare
        connect(ui->active_stareCheckBox,SIGNAL(toggled(bool)),ui->stareGroupBox,SLOT(setHidden(bool)));
        // data adaugare contract
        connect(ui->active_data_add_contractCheckBox,SIGNAL(toggled(bool)),ui->dataAdaugareContractGroupBox,SLOT(setHidden(bool)));
        // data adaugare linie
        connect(ui->active_data_add_linieCheckBox,SIGNAL(toggled(bool)),ui->dataAdaugareLinieGroupBox,SLOT(setHidden(bool)));
        // data completare
        connect(ui->active_data_completareCheckBox,SIGNAL(toggled(bool)),ui->dataAdaugareCompletareGroupBox,SLOT(setHidden(bool)));
        // data receptie
        connect(ui->active_data_receptieCheckBox,SIGNAL(toggled(bool)),ui->dataReceptieGroupBox,SLOT(setHidden(bool)));
        // cantitate
        connect(ui->active_cantitateCheckBox,SIGNAL(toggled(bool)),ui->cantitateGroupBox,SLOT(setHidden(bool)));
        // cantitate receptionata
        connect(ui->active_cantitate_receptieCheckBox,SIGNAL(toggled(bool)),ui->cantitateReceptieGroupBox,SLOT(setHidden(bool)));
        // termen
        connect(ui->active_termenCheckBox,SIGNAL(toggled(bool)),ui->termenGroupBox,SLOT(setHidden(bool)));
        // pret achizitie
        connect(ui->active_pret_aCheckBox,SIGNAL(toggled(bool)),ui->pretAchGroupBox,SLOT(setHidden(bool)));
        // pret vanzare
        connect(ui->active_pret_vCheckBox,SIGNAL(toggled(bool)),ui->pretVanzGroupBox,SLOT(setHidden(bool)));
        // programare
        connect(ui->active_programareCheckBox,SIGNAL(toggled(bool)),ui->programareGroupBox,SLOT(setHidden(bool)));
        // contactat
        connect(ui->active_contactatCheckBox,SIGNAL(toggled(bool)),ui->contactatGroupBox,SLOT(setHidden(bool)));
    }
}

void AddRaport::viewRaport() {
    QVariantMap json;
    QUrlQuery url_query;

    json.insert("colums",this->getColumns());
    json.insert("active",this->getActiveWidgets());

    url_query.addQueryItem("user_hash",The::user_hash());
    url_query.addQueryItem("data",The::serializer()->serialize(json));
    url_query.addQueryItem("tot_contractul",ui->totContractulCheckBox->isChecked()?"1":"0");

    emit query(url_query.query(QUrl::FullyEncoded).toUtf8().replace("+","%2B"));
}

QVariantList AddRaport::getDiabledWidgets() {
    QVariantList list;
    QVariantMap linie;

    foreach (const QCheckBox *object, ui->activeGeneralGroupBox->findChildren<QCheckBox*>()) {
        if (object->isChecked()) {
            linie.clear();
            linie.insert("widget",object->objectName());
            list << linie;
        }
    }
    foreach (const QCheckBox *object, ui->activaDataGroupBox->findChildren<QCheckBox*>()) {
        if (object->isChecked()) {
            linie.clear();
            linie.insert("widget",object->objectName());
            list << linie;
        }
    }
    foreach (const QCheckBox *object, ui->activeRestulGroupBox->findChildren<QCheckBox*>()) {
        if (object->isChecked()) {
            linie.clear();
            linie.insert("widget",object->objectName());
            list << linie;
        }
    }
    return list;
}

QVariantList AddRaport::getActiveWidgets() {
    QVariantList list;
    QVariantMap line;

    // ---------- contract where ----------
    if (ui->contractCheckBox->isChecked()){
        line.clear();
        line.insert("widget",ui->contractCheckBox->objectName());
        line.insert("gui_values", ui->contractLineEdit->text().trimmed());
        line.insert("sql_where",QString("(contracte.contract LIKE '%1')").arg(ui->contractLineEdit->text().trimmed()));
        list << line;
    }
    // ---------- end contract where ----------

    // ---------- auto ----------
    if (ui->autoCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->autoCheckBox->objectName());
        line.insert("gui_values", ui->autoLineEdit->text().trimmed());
        line.insert("sql_where",QString("((contracte.auto LIKE '%1') OR (contracte.numar_auto LIKE '%1'))").arg(ui->autoLineEdit->text().trimmed()));
        list << line;
    }
    // ---------- end auto ----------

    // ---------- denumire ----------
    if (ui->denumireCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->denumireCheckBox->objectName());
        line.insert("gui_values", ui->denumireLineEdit->text().trimmed());
        line.insert("sql_where",QString("(linii.denumire LIKE '%1')").arg(ui->denumireLineEdit->text().trimmed()));
        list << line;
    }
    // ---------- end denumire ----------

    // ---------- oe_am ----------
    if (ui->oe_amCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->oe_amCheckBox->objectName());
        line.insert("gui_values",ui->oe_amComboBox->currentIndex());
        line.insert("sql_where", QString("(linii.oe_am = '%1')").arg(ui->oe_amComboBox->currentText().trimmed()));
        list << line;
    }
    // ---------- end oe_am ----------

    // ---------- consilier contract ----------
    if (ui->consilier_contractCheckBox->isChecked()) {
        QStringList cons;
        line.clear();
        line.insert("widget",ui->consilier_contractCheckBox->objectName());
        line.insert("gui_values",ui->consilier_contractComboBox->checkedIndexes().join("|"));

        foreach(const QString &con, ui->consilier_contractComboBox->checkedItems()) {
            cons << QString("%1").arg(this->cons.key(con));
        }
        if (cons.length() > 1) {
            line.insert("sql_where",QString("(contracte.consilier_id IN (%1))").arg(cons.join(", ")));
        } else if (cons.length() == 1) {
            line.insert("sql_where",QString("(contracte.consilier_id = %1)").arg(cons.at(0)));
        }
        list << line;
    }
    // ---------- end consilier contract ----------

    // ---------- consilier lucrat ----------
    if (ui->consilier_lucratCheckBox->isChecked()) {
        QStringList cons;
        line.clear();
        line.insert("widget",ui->consilier_lucratCheckBox->objectName());
        line.insert("gui_values",ui->consilier_lucratComboBox->checkedIndexes().join("|"));

        foreach(const QString &con, ui->consilier_lucratComboBox->checkedItems()) {
            cons << QString("%1").arg(this->cons.key(con));
        }
        if (cons.length() > 1) {
            line.insert("sql_where",QString("(linii.consilier_id IN (%1))").arg(cons.join(", ")));
        } else if (cons.length() == 1) {
            line.insert("sql_where",QString("(linii.consilier_id = %1)").arg(cons.at(0)));
        }
        list << line;
    }
    // ---------- end consilier lucrat ----------

    // ---------- asigurator ----------
    if (ui->asiguratorCheckBox->isChecked()) {
        QStringList as;
        line.clear();
        line.insert("widget",ui->asiguratorCheckBox->objectName());
        line.insert("gui_values",ui->asiguratorComboBox->checkedIndexes().join("|"));
        foreach(const QString &a, ui->asiguratorComboBox->checkedItems()) {
            as << QString("'%1'").arg(a);
        }
        if (as.contains("'--FARA--'")) {
            line.insert("sql_where",QString("((linii.asigurator IS NULL) OR (linii.asigurator IN (%1)))")
                                            .arg(as.join(", ")));
        } else {
            line.insert("sql_where", QString("(linii.asigurator IN (%1))").arg(as.join(", ")));
        }
        list << line;
    }
    // ---------- end asigurator ----------

    // ---------- furnizor ----------
    if (ui->furnizorCheckBox->isChecked()) {
        QStringList f;
        line.clear();
        line.insert("widget",ui->furnizorCheckBox->objectName());
        line.insert("gui_values",ui->furnizorComboBox->checkedIndexes().join("|"));
        foreach(const QString &fu, ui->furnizorComboBox->checkedItems()) {
            f << QString("'%1'").arg(fu);
        }
        if (f.length()) {
            line.insert("sql_where", QString("(linii.furnizor IN (%1))").arg(f.join(", ")));
        }
        list << line;
    }
    // ---------- end furnizor ----------

    // ---------- cod ----------
    if (ui->codCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->codCheckBox->objectName());
        line.insert("gui_values", ui->codLineEdit->text().trimmed());
        line.insert("sql_where",QString("(linii.cod LIKE '%1')").arg(ui->codLineEdit->text().trimmed()));
        list << line;
    }
    // ---------- end cod ----------

    // ---------- cod cross ----------
    if (ui->cod_crossCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->cod_crossCheckBox->objectName());
        line.insert("gui_values", ui->cod_crossLineEdit->text().trimmed());
        line.insert("sql_where",QString("(linii.cod_cross LIKE '%1')").arg(ui->cod_crossLineEdit->text().trimmed()));
        list << line;
    }
    // ---------- end cod cross ----------

    // ---------- user adaugare contract ----------
    if (ui->user_add_contractCheckBox->isChecked()) {
        QStringList user;
        line.clear();
        line.insert("widget",ui->user_add_contractCheckBox->objectName());
        line.insert("gui_values",ui->user_add_contractComboBox->checkedIndexes().join("|"));

        foreach(const QString &con, ui->user_add_contractComboBox->checkedItems()) {
            user << QString("%1").arg(this->users.key(con));
        }
        if (user.length() > 1) {
            line.insert("sql_where",QString("(contracte.user_id IN (%1))").arg(user.join(", ")));
        } else if (user.length() == 1) {
            line.insert("sql_where",QString("(contracte.user_id = %1)").arg(user.at(0)));
        }
        list << line;
    }
    // ---------- end user adaugare contract ----------

    // ---------- user adaugare linie ----------
    if (ui->user_add_linieCheckBox->isChecked()) {
        QStringList user;
        line.clear();
        line.insert("widget",ui->user_add_linieCheckBox->objectName());
        line.insert("gui_values",ui->user_add_linieComboBox->checkedIndexes().join("|"));

        foreach(const QString &con, ui->user_add_linieComboBox->checkedItems()) {
            user << QString("%1").arg(this->users.key(con));
        }
        if (user.length() > 1) {
            line.insert("sql_where",QString("(linii.user_id IN (%1))").arg(user.join(", ")));
        } else if (user.length() == 1) {
            line.insert("sql_where",QString("(linii.user_id = %1)").arg(user.at(0)));
        }
        list << line;
    }
    // ---------- end user adaugare linie ----------

    // ---------- user magazie ----------
    if (ui->user_inCheckBox->isChecked()) {
        QStringList user;
        line.clear();
        line.insert("widget",ui->user_inCheckBox->objectName());
        line.insert("gui_values",ui->user_magazieComboBox->checkedIndexes().join("|"));

        foreach(const QString &con, ui->user_magazieComboBox->checkedItems()) {
            user << QString("%1").arg(this->users.key(con));
        }
        if (user.length() > 1) {
            line.insert("sql_where",QString("(linii.user_receptie IN (%1))").arg(user.join(", ")));
        } else if (user.length() == 1) {
            line.insert("sql_where",QString("(linii.user_receptie = %1)").arg(user.at(0)));
        }
        list << line;
    }
    // ---------- end user magazie ----------

    // ---------- locatie ----------
    if (ui->locatieCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->locatieCheckBox->objectName());
        line.insert("gui_values", ui->locatieLineEdit->text().trimmed());
        line.insert("sql_where",QString("(linii.locatie LIKE '%1')").arg(ui->locatieLineEdit->text().trimmed()));
        list << line;
    }
    // ---------- end locatie ----------

    // ---------- oferta/comanda ----------
    if (ui->oferta_comandaCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->oferta_comandaCheckBox->objectName());
        line.insert("gui_values", ui->oferta_comandaComboBox->currentIndex());
        line.insert("sql_where",QString("(linii.comanda = '%1')").arg(ui->oferta_comandaComboBox->currentIndex()?"T":"F"));
        list << line;
    }
    // ---------- end oferta/comanda ----------

    // ---------- finalizat ----------
    if (ui->finalizatCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->finalizatCheckBox->objectName());
        line.insert("gui_values",ui->finalizatComboBox->currentIndex());
        line.insert("sql_where",QString("(contracte.finalizat = '%1')").arg(ui->finalizatComboBox->currentIndex()?"T":"F"));
        list << line;
    }
    // ---------- end finalizat ----------

    // ---------- observatii ----------
    if (ui->observatiiCheckBox->isChecked()) {
        line.clear();
        line.insert("widget", ui->observatiiCheckBox->objectName());
        line.insert("gui_values", ui->observatiiLineEdit->text().trimmed());
        line.insert("sql_where",QString("(contracte.observatii LIKE '%1')").arg(ui->observatiiLineEdit->text().trimmed()));
        list << line;
    }
    // ---------- end observatii ----------


    // ---------- motiv ----------
    if (ui->motivCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->motivCheckBox->objectName());
        line.insert("gui_values",ui->motivComboBox->currentText());
        line.insert("sql_where",QString("(contracte.motiv = '%1')").arg(ui->motivComboBox->currentText().trimmed()));
        list << line;
    }
    // ---------- end motiv ----------

    // ---------- stare ----------
    line.clear();
    line.insert("widget",ui->stareGroupBox->objectName());
    QStringList stare;
    QStringList where_stare;
    if (ui->stare_intrare_nouaCheckBox->isChecked()) {
        stare << ui->stare_intrare_nouaCheckBox->objectName();
        where_stare << "((linii.stare = 0) AND (linii.sters = 'F'))";
    }
    if (ui->stare_complectateCheckBox->isChecked()) {
        stare << ui->stare_complectateCheckBox->objectName();
        where_stare << "((linii.stare = 1) AND (linii.sters = 'F'))";
    }
    if (ui->stare_revazutaCheckBox->isChecked()) {
        stare << ui->stare_revazutaCheckBox->objectName();
        where_stare << "((linii.stare = 2) AND (linii.sters = 'F'))";
    }
    if (ui->stare_comandataCheckBox->isChecked()) {
        stare << ui->stare_comandataCheckBox->objectName();
        where_stare << "((linii.stare = 3) AND (linii.sters = 'F'))";
    }
    if (ui->stare_piese_veniteCheckBox->isChecked()) {
        if (ui->partialCheckBox->isChecked()) {
            stare << ui->stare_piese_veniteCheckBox->objectName() + "|" + ui->partialCheckBox->objectName();
            where_stare << "((linii.stare = 3) AND (linii.cantitate_in > 0) AND (linii.sters = 'F'))";
        } else {
            stare << ui->stare_piese_veniteCheckBox->objectName();
            where_stare << "((linii.stare = 4) AND (linii.sters = 'F'))";
        }
    }
    if (ui->piese_neveniteCheckBox->isChecked()) {
        if (ui->nevenite_partialCheckBox->isChecked()) {
            stare << ui->piese_neveniteCheckBox->objectName() + "|" + ui->nevenite_partialCheckBox->objectName();
            where_stare << "((linii.stare = 3) AND (linii.sters = 'F') AND "
                           "(linii.cantitate_in < linii.cantitate))";

        } else {
            stare << ui->piese_neveniteCheckBox->objectName();
            where_stare << "((linii.stare = 3) AND (linii.sters = 'F') AND "
                   "((linii.cantitate_in = 0) OR (linii.cantitate_in IS NULL)))";
        }
    }
    if (ui->stare_liniiCheckBox->isChecked()) {
        stare << ui->stare_liniiCheckBox->objectName();
        where_stare << "(linii.sters = 'T')";
    }
    line.insert("gui_values",stare.join("|"));
    line.insert("sql_where", QString("(%1)").arg(where_stare.join(" OR ")));
    if (stare.length()) {
        list << line;
    }
    // ---------- end stare ----------


    // -------------------- DATA ADAUGARE CONTRACT --------------------
    // ----------  azi ----------
    if (ui->date_add_aziCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_aziCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(DATEDIFF(contracte.date_added,NOW()) = 0)");
        list << line;
    }
    // ---------- end azi ----------

    // ---------- luna asta ----------
    if (ui->date_add_lunaCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_lunaCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), "
                    "EXTRACT(YEAR_MONTH FROM contracte.date_added)) = 0)");
        list << line;
    }
    // ---------- end luna asta ----------

    // ---------- acum zile ----------
    if (ui->date_add_acum_zileCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_acum_zileCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->date_add_acum_zileSpinBox->value())
                                                 .arg(ui->date_add_acum_zile_intervalCheckBox->isChecked()?1:0));
        if (ui->date_add_acum_zile_intervalCheckBox->isChecked()) {
            line.insert("sql_where", QString("((DATEDIFF(NOW(),contracte.date_added) <= %1) AND (DATEDIFF(NOW(),contracte.date_added) >= 0))")
                                             .arg(ui->date_add_acum_zileSpinBox->value()));
        } else {
            line.insert("sql_where", QString("(DATEDIFF(NOW(),contracte.date_added) = %1)")
                                             .arg(ui->date_add_acum_zileSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum zile ----------

    // ---------- acum luni ----------
    if (ui->date_add_acum_luniCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_acum_luniCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->date_add_acum_luniSpinBox->value())
                                         .arg(ui->date_add_acum_luni_intervalCheckBox->isChecked()?1:0));
        if (ui->date_add_acum_luni_intervalCheckBox->isChecked()) {
            line.insert("sql_where",QString("((PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM contracte.date_added)) <= %1) AND "
                                             "(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM contracte.date_added)) >= 0))")
                                             .arg(ui->date_add_acum_luniSpinBox->value()));
        } else {
            line.insert("sql_where",QString("(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM contracte.date_added)) = %1)")
                                             .arg(ui->date_add_acum_luniSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum zile ----------

    // ---------- intre ----------
    if (ui->date_add_intreCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_intreCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2")
                                        .arg(ui->date_add_minDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                                        .arg(ui->date_add_maxDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        line.insert("sql_where",QString("((contracte.date_added >= '%1') AND (contracte.date_added <= '%2'))")
                .arg(ui->date_add_minDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                .arg(ui->date_add_maxDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        list << line;
    }
    // ---------- end intre ----------
    // -------------------- END DATA ADAUGARE CONTRACT --------------------


    // -------------------- DATA ADAUGARE LINIE --------------------
    // ---------- azi ----------
    if (ui->date_add_azi_linieCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_azi_linieCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(DATEDIFF(linii.date_added,NOW()) = 0)");
        list << line;
    }
    // ---------- end azi ----------

    // ---------- luna asta ----------
    if (ui->date_add_luna_linieCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_luna_linieCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), "
                    "EXTRACT(YEAR_MONTH FROM linii.date_added)) = 0)");
        list << line;
    }
    // ---------- end luna asta ----------

    // ---------- acum zile ----------
    if (ui->date_add_acum_zile_linieCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_acum_zile_linieCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->date_add_acum_zile_linieSpinBox->value())
                                                 .arg(ui->date_add_acum_zile_interval_linieCheckBox->isChecked()?1:0));
        if (ui->date_add_acum_zile_intervalCheckBox->isChecked()) {
            line.insert("sql_where", QString("((DATEDIFF(NOW(),linii.date_added) <= %1) AND (DATEDIFF(NOW(), linii.date_added) >= 0))")
                                             .arg(ui->date_add_acum_zile_linieSpinBox->value()));
        } else {
            line.insert("sql_where", QString("(DATEDIFF(NOW(),linii.date_added) = %1)")
                                             .arg(ui->date_add_acum_zile_linieSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum zile ----------

    // ---------- acum luni ----------
    if (ui->date_add_acum_luni_linieCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_acum_luni_linieCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->date_add_acum_luni_linieSpinBox->value())
                                         .arg(ui->date_add_acum_luni_interval_linieCheckBox->isChecked()?1:0));
        if (ui->date_add_acum_luni_interval_linieCheckBox->isChecked()) {
            line.insert("sql_where",QString("((PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.date_added)) <= %1) AND "
                                             "(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.date_added)) >= 0))")
                                             .arg(ui->date_add_acum_luni_linieSpinBox->value()));
        } else {
            line.insert("sql_where",QString("(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.date_added)) = %1)")
                                             .arg(ui->date_add_acum_luni_linieSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum zile ----------

    // ---------- intre ----------
    if (ui->date_add_intre_linieCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_add_intre_linieCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2")
                                        .arg(ui->date_add_min_linieDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                                        .arg(ui->date_add_max_linieDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        line.insert("sql_where",QString("((linii.date_added >= '%1') AND (linii.date_added <= '%2'))")
                                        .arg(ui->date_add_min_linieDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                                        .arg(ui->date_add_max_linieDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        list << line;
    }
    // ---------- end intre ----------
    // ---------- END DATA ADAUGARE LINIE ----------


    // -------------------- DATA COMPLETARE CONTRACT --------------------
    // ---------- azi ----------
    if (ui->date_edit_aziCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_edit_aziCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(DATEDIFF(linii.data_edit,NOW()) = 0)");
        list << line;
    }
    // ---------- end azi ----------

    // ---------- luna asta ----------
    if (ui->date_edit_lunaCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_edit_lunaCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), "
                    "EXTRACT(YEAR_MONTH FROM linii.data_edit)) = 0)");
        list << line;
    }
    // ---------- end luna asta ----------

    // ---------- acum zile ----------
    if (ui->date_edit_acum_zileCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_edit_acum_zileCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->date_edit_acum_zileSpinBox->value())
                                                 .arg(ui->date_edit_acum_zile_intervalCheckBox->isChecked()?1:0));
        if (ui->date_edit_acum_zile_intervalCheckBox->isChecked()) {
            line.insert("sql_where", QString("((DATEDIFF(NOW(),linii.data_edit) <= %1) AND (DATEDIFF(NOW(),linii.data_edit) >= 0))")
                                             .arg(ui->date_edit_acum_zileSpinBox->value()));
        } else {
            line.insert("sql_where", QString("(DATEDIFF(NOW(),linii.data_edit) = %1)")
                                             .arg(ui->date_edit_acum_zileSpinBox->value()));
        }
        list << line;
    }
    // ---------- end data adaugare contract acum zile ----------

    // ---------- acum luni ----------
    if (ui->date_edit_acum_luniCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_edit_acum_luniCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->date_edit_acum_luniSpinBox->value())
                                         .arg(ui->date_edit_acum_luni_intervalCheckBox->isChecked()?1:0));
        if (ui->date_edit_acum_luni_intervalCheckBox->isChecked()) {
            line.insert("sql_where",QString("((PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.data_edit)) <= %1) AND "
                                             "(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.data_edit)) >= 0))")
                                             .arg(ui->date_edit_acum_luniSpinBox->value()));
        } else {
            line.insert("sql_where",QString("(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.data_edit)) = %1)")
                                             .arg(ui->date_edit_acum_luniSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum luni ----------

    // ---------- intre ----------
    if (ui->date_edit_intreCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_edit_intreCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2")
                                        .arg(ui->date_edit_minDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                                        .arg(ui->date_edit_maxDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        line.insert("sql_where",QString("((linii.data_edit >= '%1') AND (linii.data_edit <= '%2'))")
                .arg(ui->date_edit_minDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                .arg(ui->date_edit_maxDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        list << line;
    }
    // ---------- end intre ----------
    // -------------------- END DATA COMPLETARE --------------------


    // -------------------- DATA RECEPTIE --------------------
    // ---------- azi ----------
    if (ui->date_receptie_aziCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_receptie_aziCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(DATEDIFF(linii.data_receptie, NOW()) = 0)");
        list << line;
    }
    // ---------- end azi ----------

    // ---------- luna asta ----------
    if (ui->date_receptie_lunaCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_receptie_lunaCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), "
                    "EXTRACT(YEAR_MONTH FROM linii.data_receptie)) = 0)");
        list << line;
    }
    // ---------- end luna asta ----------

    // ---------- acum zile ----------
    if (ui->date_receptie_acum_zileCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_receptie_acum_zileCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->date_receptie_acum_zileSpinBox->value())
                                                 .arg(ui->date_receptie_acum_zile_intervalCheckBox->isChecked()?1:0));
        if (ui->date_receptie_acum_zile_intervalCheckBox->isChecked()) {
            line.insert("sql_where", QString("((DATEDIFF(NOW(),linii.data_receptie) <= %1) AND (DATEDIFF(NOW(),linii.data_receptie) >= 0))")
                                             .arg(ui->date_receptie_acum_zileSpinBox->value()));
        } else {
            line.insert("sql_where", QString("(DATEDIFF(NOW(),linii.data_receptie) = %1)")
                                             .arg(ui->date_receptie_acum_zileSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum zile ----------

    // ---------- acum luni ----------
    if (ui->date_receptie_acum_luniCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_receptie_acum_luniCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->date_receptie_acum_luniSpinBox->value())
                                         .arg(ui->date_receptie_acum_luni_intervalCheckBox->isChecked()?1:0));
        if (ui->date_receptie_acum_luni_intervalCheckBox->isChecked()) {
            line.insert("sql_where",QString("((PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.data_receptie)) <= %1) AND "
                                             "(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.data_receptie)) >= 0))")
                                             .arg(ui->date_receptie_acum_luniSpinBox->value()));
        } else {
            line.insert("sql_where",QString("(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM linii.data_receptie)) = %1)")
                                             .arg(ui->date_receptie_acum_luniSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum luni ----------

    // ---------- intre ----------
    if (ui->date_receptie_intreCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->date_receptie_intreCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2")
                                        .arg(ui->date_receptie_minDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                                        .arg(ui->date_receptie_maxDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        line.insert("sql_where",QString("((linii.data_receptie >= '%1') AND (linii.data_receptie <= '%2'))")
                .arg(ui->date_receptie_minDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                .arg(ui->date_receptie_maxDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        list << line;
    }
    // ---------- end intre ----------
    // -------------------- END RECEPTIE --------------------


    // -------------------- CANTITATE --------------------
    // ---------- cantitate = ----------
    if (ui->cantitate_eqCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->cantitate_eqCheckBox->objectName());
        line.insert("gui_values",ui->cantitate_eqSpinBox->value());
        line.insert("sql_where",QString("(linii.cantitate = %1)").arg(ui->cantitate_eqSpinBox->value()));
        list << line;
    }
    // ---------- end cantitate = ----------

    // ---------- cantitate > ----------
    if (ui->cantitate_gtCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->cantitate_gtCheckBox->objectName());
        line.insert("gui_values",ui->cantitate_gtSpinBox->value());
        line.insert("sql_where",QString("(linii.cantitate > %1)").arg(ui->cantitate_eqSpinBox->value()));
        list << line;
    }
    // ---------- end cantitate > ----------

    // ---------- cantitate < ----------
    if (ui->cantitate_ltCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->cantitate_ltCheckBox->objectName());
        line.insert("gui_values",ui->cantitate_ltSpinBox->value());
        line.insert("sql_where",QString("(linii.cantitate < %1)").arg(ui->cantitate_ltSpinBox->value()));
        list << line;
    }
    // ---------- end cantitate < ----------
    // -------------------- END CANTITATE --------------------


    // -------------------- CANTITATE RECEPTIE --------------------
    // ---------- cantitate = ----------
    if (ui->cantitate_in_eqCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->cantitate_in_eqCheckBox->objectName());
        line.insert("gui_values",ui->cantitate_in_eqSpinBox->value());
        line.insert("sql_where",QString("(linii.cantitate_in = %1)").arg(ui->cantitate_in_eqSpinBox->value()));
        list << line;
    }
    // ---------- end cantitate = ----------

    // ---------- cantitate > ----------
    if (ui->cantitate_in_gtCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->cantitate_in_gtCheckBox->objectName());
        line.insert("gui_values",ui->cantitate_in_gtSpinBox->value());
        line.insert("sql_where",QString("(linii.cantitate_in > %1)").arg(ui->cantitate_in_gtSpinBox->value()));
        list << line;
    }
    // ---------- end cantitate > ----------

    // ---------- cantitate < ----------
    if (ui->cantitate_in_ltCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->cantitate_in_ltCheckBox->objectName());
        line.insert("gui_values",ui->cantitate_in_ltSpinBox->value());
        line.insert("sql_where",QString("(linii.cantitate < %1)").arg(ui->cantitate_in_ltSpinBox->value()));
        list << line;
    }
    // ---------- end cantitate < ----------
    // -------------------- END CANTITATE RECEPTIE --------------------


    // -------------------- TERMEN --------------------
    // ---------- termen = ----------
    if (ui->termen_eqCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->termen_eqCheckBox->objectName());
        line.insert("gui_values",ui->termen_eqSpinBox->value());
        line.insert("sql_where",QString("(linii.termen = %1)").arg(ui->termen_eqSpinBox->value()));
        list << line;
    }
    // ---------- end termen = ----------

    // ----------  termen > ----------
    if (ui->termen_gtCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->termen_gtCheckBox->objectName());
        line.insert("gui_values",ui->termen_gtSpinBox->value());
        line.insert("sql_where",QString("(linii.termen > %1)").arg(ui->termen_gtSpinBox->value()));
        list << line;
    }
    // ---------- end termen > ----------

    // ---------- end termen < ----------
    if (ui->termen_ltCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->termen_ltCheckBox->objectName());
        line.insert("gui_values",ui->termen_ltSpinBox->value());
        line.insert("sql_where",QString("(linii.termen < %1)").arg(ui->termen_ltSpinBox->value()));
        list << line;
    }
    // ---------- end termen < ----------

    // ---------- sosire ----------
    if (ui->termen_sosireCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->termen_sosireCheckBox->objectName());
        line.insert("gui_values",ui->termen_sosireSpinBox->value());
        line.insert("sql_where",QString("(DATEDIFF( "
                                        "IF( linii.termen + WEEKDAY(linii.data_edit) < 5, "
                                        "DATE_ADD(linii.data_edit, INTERVAL linii.termen DAY), "
                                        "CASE "
                                           "WEEKDAY(linii.data_edit) "
                                           "WHEN 5 THEN DATE_ADD(linii.data_edit, INTERVAL ( ((linii.termen + 2 ) DIV 5) *7 + ((linii.termen + 2 ) MOD 5)) DAY) "
                                           "WHEN 6 THEN DATE_ADD(linii.data_edit, INTERVAL ( ((linii.termen + 1 ) DIV 5) *7 + ((linii.termen + 1 ) MOD 5)) DAY) "
                                           "WHEN 0 THEN DATE_ADD(linii.data_edit, INTERVAL ( (linii.termen DIV 5) *7 + (linii.termen MOD 5)) DAY) "
                                           "WHEN 1 THEN DATE_ADD(DATE_ADD(linii.data_edit, INTERVAL 5 DAY), INTERVAL (((linii.termen - 3 ) DIV 5) *7 + ((linii.termen - 3 ) MOD 5)) DAY) "
                                           "WHEN 2 THEN DATE_ADD(DATE_ADD(linii.data_edit, INTERVAL 4 DAY), INTERVAL (((linii.termen - 2 ) DIV 5) *7 + ((linii.termen - 2 ) MOD 5)) DAY) "
                                           "WHEN 3 THEN DATE_ADD(DATE_ADD(linii.data_edit, INTERVAL 3 DAY), INTERVAL (((linii.termen - 1 ) DIV 5) *7 + ((linii.termen - 1 ) MOD 5)) DAY) "
                                           "WHEN 4 THEN DATE_ADD(DATE_ADD(linii.data_edit, INTERVAL 2 DAY), INTERVAL (((linii.termen - 0 ) DIV 5) *7 + ((linii.termen - 0 ) MOD 5)) DAY) "
                                           "ELSE NULL "
                                         " END),NOW()) = %1)").arg(ui->termen_sosireSpinBox->value()));
        list << line;
    }
    // ---------- end sosire ----------

    // ---------- termen depasit ----------
    if (ui->termen_depasitCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->termen_depasitCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where",QString("(DATEDIFF( "
                                        "IF( linii.termen + WEEKDAY(linii.data_edit) < 5, "
                                        "DATE_ADD(linii.data_edit, INTERVAL linii.termen DAY), "
                                        "CASE "
                                           "WEEKDAY(linii.data_edit) "
                                           "WHEN 5 THEN DATE_ADD(linii.data_edit, INTERVAL ( ((linii.termen + 2 ) DIV 5) *7 + ((linii.termen + 2 ) MOD 5)) DAY) "
                                           "WHEN 6 THEN DATE_ADD(linii.data_edit, INTERVAL ( ((linii.termen + 1 ) DIV 5) *7 + ((linii.termen + 1 ) MOD 5)) DAY) "
                                           "WHEN 0 THEN DATE_ADD(linii.data_edit, INTERVAL ( (linii.termen DIV 5) *7 + (linii.termen MOD 5)) DAY) "
                                           "WHEN 1 THEN DATE_ADD(DATE_ADD(linii.data_edit, INTERVAL 5 DAY), INTERVAL (((linii.termen - 3 ) DIV 5) *7 + ((linii.termen - 3 ) MOD 5)) DAY) "
                                           "WHEN 2 THEN DATE_ADD(DATE_ADD(linii.data_edit, INTERVAL 4 DAY), INTERVAL (((linii.termen - 2 ) DIV 5) *7 + ((linii.termen - 2 ) MOD 5)) DAY) "
                                           "WHEN 3 THEN DATE_ADD(DATE_ADD(linii.data_edit, INTERVAL 3 DAY), INTERVAL (((linii.termen - 1 ) DIV 5) *7 + ((linii.termen - 1 ) MOD 5)) DAY) "
                                           "WHEN 4 THEN DATE_ADD(DATE_ADD(linii.data_edit, INTERVAL 2 DAY), INTERVAL (((linii.termen - 0 ) DIV 5) *7 + ((linii.termen - 0 ) MOD 5)) DAY) "
                                           "ELSE NULL "
                                         " END),NOW()) < 0)"));
        list << line;
    }
    // ---------- end termen depasit ----------
    // -------------------- END TERMEN --------------------


    // -------------------- PRET ACHIZITIE --------------------
    // ---------- pret achh = ----------
    if (ui->pret_a_eqCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->pret_a_eqCheckBox->objectName());
        line.insert("gui_values",ui->pret_a_eqSpinBox->value());
        line.insert("sql_where",QString("(linii.pret_achizitie = %1)").arg(ui->pret_a_eqSpinBox->value()));
        list << line;
    }
    // ---------- end pret achh = ----------

    // ---------- pret achh > ----------
    if (ui->pret_a_gtCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->pret_a_gtCheckBox->objectName());
        line.insert("gui_values",ui->pret_a_gtSpinBox->value());
        line.insert("sql_where",QString("(linii.pret_achizitie > %1)").arg(ui->pret_a_gtSpinBox->value()));
        list << line;
    }
    // ---------- end pret achh > ----------

    // ---------- pret achh < ----------
    if (ui->pret_a_ltCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->pret_a_ltCheckBox->objectName());
        line.insert("gui_values",ui->pret_a_ltSpinBox->value());
        line.insert("sql_where",QString("(linii.pret_achizitie < %1)").arg(ui->pret_a_ltSpinBox->value()));
        list << line;
    }
    // ---------- end achh < ----------
    // -------------------- END PRET ACHIZITIE --------------------


    // -------------------- PRET VANZARE --------------------
    // ---------- pret achh = ----------
    if (ui->pret_v_eqCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->pret_v_eqCheckBox->objectName());
        line.insert("gui_values",ui->pret_v_eqSpinBox->value());
        line.insert("sql_where",QString("(linii.pret_vanzare = %1)").arg(ui->pret_v_eqSpinBox->value()));
        list << line;
    }
    // ---------- end pret achh = ----------

    // ---------- pret achh > ----------
    if (ui->pret_v_gtCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->pret_v_gtCheckBox->objectName());
        line.insert("gui_values",ui->pret_v_gtSpinBox->value());
        line.insert("sql_where",QString("(linii.pret_vanzare > %1)").arg(ui->pret_v_gtSpinBox->value()));
        list << line;
    }
    // ---------- end pret achh > ----------

    // ---------- pret achh < ----------
    if (ui->pret_v_ltCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->pret_v_ltCheckBox->objectName());
        line.insert("gui_values",ui->pret_v_ltSpinBox->value());
        line.insert("sql_where",QString("(linii.pret_vanzare < %1)").arg(ui->pret_v_ltSpinBox->value()));
        list << line;
    }
    // ---------- end achh < ----------
    // -------------------- END PRET VANZARE --------------------


    // -------------------- PROGRAMARE --------------------
    // ---------- azi ----------
    if (ui->programare_aziCheckBox->isChecked()){
        line.clear();
        line.insert("widget",ui->programare_aziCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(DATEDIFF(contracte.programare,NOW()) = 0)");
        list << line;
    }
    // ---------- end azi ----------

    // ---------- maine ----------
    if (ui->programare_aziCheckBox->isChecked()){
        line.clear();
        line.insert("widget",ui->programare_aziCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(DATEDIFF(contracte.programare,NOW()) = 1)");
        list << line;
    }
    // ---------- end maine ----------

    // ---------- luna ----------
    if (ui->programare_lunaCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->programare_lunaCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where","(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM contracte.programare)) = 0)");
        list << line;
    }
    // ---------- end luna ----------

    // ---------- acum zile ----------
    if (ui->programare_acum_zileCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->programare_acum_zileCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->programare_acum_zileSpinBox->value())
                                         .arg(ui->programare_acum_zile_intervalCheckBox->isChecked()?1:0));
        if (ui->programare_acum_zile_intervalCheckBox->isChecked()) {
            line.insert("sql_where",QString("((DATEDIFF(NOW(),contracte.programare) <= %1) AND () (DATEDIFF(NOW(),contracte.programare) >= 0)")
                                            .arg(ui->programare_acum_zileSpinBox->value()));
        } else {
            line.insert("sql_where",QString("(DATEDIFF(NOW(),contracte.programare) = %1)")
                                            .arg(ui->programare_acum_zileSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum zile ----------

    // ---------- acum luni ----------
    if (ui->programare_acum_luniCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->programare_acum_luniCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->programare_acum_luniSpinBox->value())
                                        .arg(ui->programare_acum_luni_intervalCheckBox->isChecked()?1:0));
        if (ui->programare_acum_luni_intervalCheckBox->isChecked()) {
            line.insert("sql_where",QString("(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM contracte.programare)) <= %1) AND "
                                            "(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM contracte.programare)) >= 0)")
                                            .arg(ui->programare_acum_luniSpinBox->value()));
        } else {
            line.insert("sql_where",QString("(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM NOW()), EXTRACT(YEAR_MONTH FROM contracte.programare)) = %1)")
                                            .arg(ui->programare_acum_luniSpinBox->value()));
        }
        list << line;
    }
    // ---------- end acum luni ----------

    // ---------- peste zile ----------
    if (ui->programare_peste_zileCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->programare_peste_zileCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->programare_peste_zileSpinBox->value())
                                        .arg(ui->programare_peste_zile_intervalCheckBox->isChecked()?1:0));
        if (ui->programare_peste_zile_intervalCheckBox->isChecked()) {
            line.insert("sql_where",QString("((DATEDIFF(contracte.programare,NOW()) <= %1) AND () (DATEDIFF(contracte.programare, NOW()) >= 0)")
                                            .arg(ui->programare_peste_zileSpinBox->value()));
        } else {
            line.insert("sql_where",QString("(DATEDIFF(contracte.programare,NOW()) = %1)")
                                            .arg(ui->programare_peste_zileSpinBox->value()));
        }
        list << line;
    }
    // ---------- end peste zile ----------

    // ---------- peste luna ----------
    if (ui->programare_peste_luniCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->programare_peste_luniCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->programare_peste_luniSpinBox->value())
                                        .arg(ui->programare_peste_luni_intervalCheckBox->isChecked()?1:0));
        if (ui->programare_peste_luni_intervalCheckBox->isChecked()) {
            line.insert("sql_where",QString("(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM contracte.programare), EXTRACT(YEAR_MONTH FROM NOW())) <= %1) AND "
                                            "(PERIOD_DIFF(EXTRACT(YEAR_MONTH FROM contracte.programare), EXTRACT(YEAR_MONTH FROM NOW())) >= 0)")
                                            .arg(ui->programare_peste_luniSpinBox->value()));
        } else {
            line.insert("sql_where",QString("((MONTH(contracte.programare) = MONTH(NOW()) + %1) AND"
                                            " YEAR(contracte.programare) = YEAR(NOW()))")
                                             .arg(ui->programare_peste_luniSpinBox->value()));
        }
        list << line;
    }
    // ---------- end peste luna ----------

    // ---------- intre ----------
    if (ui->programare_intreCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->programare_intreCheckBox->objectName());
        line.insert("gui_values",QString("%1|%2").arg(ui->programare_intre_minDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                                        .arg(ui->programare_intre_maxDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        line.insert("sql_where", QString("(contracte.programare >= '%1' AND contracte.programare <= '%2')")
                                    .arg(ui->programare_intre_minDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00"))
                                    .arg(ui->programare_intre_maxDateTimeEdit->dateTime().toString("yyyy-MM-dd  HH:mm:00")));
        list << line;
    }
    // ---------- end intre ----------

    // ---------- fara programare ----------
    if (ui->programare_faraCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->programare_faraCheckBox->objectName());
        line.insert("gui_values",1);
        line.insert("sql_where", "(contracte.programare IS NULL)");
        list << line;
    }
    // ---------- end fara programare ----------
    // -------------------- END PROGRAMARE --------------------

    // ---------- contactat ----------
    if (ui->contactatCheckBox->isChecked()) {
        line.clear();
        line.insert("widget",ui->contactatCheckBox->objectName());
        line.insert("gui_values", ui->contactatComboBox->currentIndex());
        line.insert("sql_where", QString("(contracte.contactat = '%1')").arg(ui->contactatComboBox->currentIndex()?"T":"F"));
        list << line;
    }
    // ---------- end contactat ----------

    return list;
}

void AddRaport::getEditData(int raport_id) {
    this->raport_id = raport_id;
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());
    query.addQueryItem("raport_id", QString("%1").arg(raport_id));

    QUrl url = QUrl(QString(The::server_url() + "rapoarte/edit_raport.json"));
    QObject *scan = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(scan, SIGNAL(data(QByteArray)),this, SLOT(parseEditData(QByteArray)));
    connect(scan, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void AddRaport::parseEditData(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");

        Log::logger()->logError("Eroare parsare JSON","AddRaport","parseData();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"AddRaport","parseData();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        ui->numeEdit->setText(result["denumire"].toString());
        if (result["tot_contractul"].toBool()) {
            ui->totContractulCheckBox->setChecked(true);
        } else {
            ui->totContractulCheckBox->setChecked(false);
        }

        // colums
        QList<QListWidgetItem*> items;
        QListWidgetItem *move;

        this->uncheckAll();
        int order = 0;
        foreach (const QVariant &colum, result["colums"].toList()) {
            items = ui->coloaneListWidget->findItems(colum.toMap()["denumire"].toString(),Qt::MatchCaseSensitive);
            if (items.count())  {
                items.at(0)->setCheckState(Qt::Checked);
                move = items.at(0);
                ui->coloaneListWidget->takeItem(ui->coloaneListWidget->row(move));
                ui->coloaneListWidget->insertItem(order,move);
                ++order;
            }
        }
        // end colums

        // disable
        QCheckBox *check;
        foreach (const QVariant &disable, result["disabled"].toList()) {
            check = ui->active->findChild<QCheckBox *>(disable.toMap()["widget"].toString());
            if (check) {
                check->animateClick(1);
            }
        }
        // end disable

        // hidden
        foreach(const QVariant &hidden, result["hidden"].toList()) {
//            qDebug() << this->cols.key(hidden.toMap()["coloana"].toString());
            items = ui->active_coloaneListWidget->findItems(this->cols.key(hidden.toMap()["coloana"].toString()),Qt::MatchCaseSensitive);
            if (items.count()) {
                items.at(0)->setCheckState(Qt::Checked);
            }
        }

        // end hidden

        //avtive
        QString i;
        QString g;
        QStringList vals;
        QDateTime dat;
        bool ok;
        int nr;
        foreach (const QVariant &ac, result["active"].toList()) {
            i = ac.toMap()["widget"].toString();
            g = ac.toMap()["gui_values"].toString();
            if (i == ui->contractCheckBox->objectName()) { // contract
                ui->contractCheckBox->setChecked(true);
                ui->contractLineEdit->setText(g);
            } else if (i == ui->autoCheckBox->objectName()) { // auto
                ui->autoCheckBox->setChecked(true);
                ui->autoLineEdit->setText(g);
            } else if (i == ui->denumireCheckBox->objectName()) { // denumire
                ui->denumireCheckBox->setChecked(true);
                ui->denumireLineEdit->setText(g);
            } else if (i == ui->oe_amCheckBox->objectName()) { // OE_AM
                ui->oe_amCheckBox->setChecked(true);
                ui->oe_amComboBox->setCurrentIndex(g.toInt());
            } else if (i == ui->consilier_contractCheckBox->objectName()) { // consilier_contract
                ui->consilier_contractCheckBox->setChecked(true);
                ui->consilier_contractComboBox->uncheckAll();
                foreach (const QString &index, g.split("|")) {
                    nr = index.toInt(&ok);
                    if (ok) {
                        ui->consilier_contractComboBox->setItemCheckState(nr,true);
                    }
                }
            } else if (i == ui->consilier_lucratCheckBox->objectName()) { // consilier_lucrat
                ui->consilier_lucratCheckBox->setChecked(true);
                ui->consilier_lucratComboBox->uncheckAll();
                foreach (const QString &index, g.split("|")) {
                    nr = index.toInt(&ok);
                    if (ok) {
                        ui->consilier_lucratComboBox->setItemCheckState(nr,true);
                    }
                }
            } else if (i == ui->asiguratorCheckBox->objectName()) { // asigurator
                ui->asiguratorCheckBox->setChecked(true);
                ui->asiguratorComboBox->uncheckAll();
                foreach (const QString &index, g.split("|")) {
                    nr = index.toInt(&ok);
                    if (ok) {
                        ui->asiguratorComboBox->setItemCheckState(nr,true);
                    }
                }
            } else if (i == ui->furnizorCheckBox->objectName()) { // furnizor
                ui->furnizorCheckBox->setChecked(true);
                ui->furnizorComboBox->uncheckAll();
                foreach (const QString &index, g.split("|")) {
                    nr = index.toInt(&ok);
                    if (ok) {
                        ui->furnizorComboBox->setItemCheckState(nr,true);
                    }
                }
            } else if (i == ui->codCheckBox->objectName()) { // cod
                ui->codCheckBox->setChecked(true);
                ui->codLineEdit->setText(g);
            } else if (i == ui->cod_crossCheckBox->objectName()) { // cod_cross
                ui->cod_crossCheckBox->setChecked(true);
                ui->cod_crossLineEdit->setText(g);
            } else if (i == ui->user_add_contractCheckBox->objectName()) { // user_add
                ui->user_add_contractCheckBox->setChecked(true);
                ui->user_add_contractComboBox->uncheckAll();
                foreach (const QString &index, g.split("|")) {
                    nr = index.toInt(&ok);
                    if (ok) {
                        ui->user_add_contractComboBox->setItemCheckState(nr,true);
                    }
                }
            } else if (i == ui->user_add_linieCheckBox->objectName()) { // user_add_linie
                ui->user_add_linieCheckBox->setChecked(true);
                ui->user_add_linieComboBox->uncheckAll();
                foreach (const QString &index, g.split("|")) {
                    nr = index.toInt(&ok);
                    if (ok) {
                        ui->user_add_linieComboBox->setItemCheckState(nr,true);
                    }
                }
            } else if (i == ui->user_inCheckBox->objectName()) { // user_in
                ui->user_inCheckBox->setChecked(true);
                ui->user_magazieComboBox->uncheckAll();
                foreach (const QString &index, g.split("|")) {
                    nr = index.toInt(&ok);
                    if (ok) {
                        ui->user_magazieComboBox->setItemCheckState(nr,true);
                    }
                }
            } else if (i == ui->locatieCheckBox->objectName()) { //locatie
                ui->locatieCheckBox->setChecked(true);
                ui->locatieLineEdit->setText(g);
            } else if (i == ui->oferta_comandaCheckBox->objectName()) { // oferta_comanda
                ui->oferta_comandaCheckBox->setChecked(true);
                ui->oferta_comandaComboBox->setCurrentIndex(g.toInt());
            } else if (i == ui->finalizatCheckBox->objectName()) { // finalizat
                ui->finalizatCheckBox->setChecked(true);
                ui->finalizatComboBox->setCurrentIndex(g.toInt());
            } else if (i == ui->observatiiCheckBox->objectName()) { // observatii
                ui->observatiiCheckBox->setChecked(true);
                ui->observatiiLineEdit->setText(g);
            } else if (i == ui->motivCheckBox->objectName()) {
                ui->motivCheckBox->setChecked(true);
                nr = g.toInt(&ok);
                if (ok) {
                    ui->motivComboBox->setCurrentIndex(nr);
                }
            } else if (i == ui->stareGroupBox->objectName()) { // stare
                foreach (const QString &chk, g.split("|")) {
                    check = ui->stareGroupBox->findChild<QCheckBox *>(chk);
                    if (check) {
                        check->setChecked(true);
                    }
                }
            } else if (i == ui->date_add_aziCheckBox->objectName()) { // data_add_azi
                ui->date_add_aziCheckBox->setChecked(true);
            } else if (i == ui->date_add_lunaCheckBox->objectName()) { // data_add_luna
                ui->date_add_lunaCheckBox->setChecked(true);
            } else if (i == ui->date_add_acum_zileCheckBox->objectName()) { // data_add_acum_zile
                ui->date_add_acum_zileCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->date_add_acum_zileSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->date_add_acum_zile_intervalCheckBox->setChecked(true);
                    } else {
                        ui->date_add_acum_zile_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->date_add_acum_luniCheckBox->objectName()) { // data_add_luni
                ui->date_add_acum_luniCheckBox->setChecked(true);
                vals.clear();
                vals  = g.split("|");
                if (vals.count() == 2) {
                    ui->date_add_acum_luniSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->date_add_acum_luni_intervalCheckBox->setChecked(true);
                    } else {
                        ui->date_add_acum_luni_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->date_add_intreCheckBox->objectName()) { // date_add_intre
                ui->date_add_intreCheckBox->setChecked(true);
                vals.clear();
                vals  = g.split("|");
                if (vals.count() == 2) {
                    ui->date_add_minDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(0),"yyyy-MM-dd  HH:mm:00"));
                    ui->date_add_maxDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(1),"yyyy-MM-dd  HH:mm:00"));
                }
            } else if (i == ui->date_add_azi_linieCheckBox->objectName()) { // date_add_azi_linie
                ui->date_add_azi_linieCheckBox->setChecked(true);
            } else if (i == ui->date_add_luna_linieCheckBox->objectName()) { // date_add_luna_linie
                ui->date_add_luna_linieCheckBox->setChecked(true);
            } else if (i == ui->date_add_acum_zile_linieCheckBox->objectName()) { // date_add_acum_zile_linie
                ui->date_add_acum_zile_linieCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->date_add_acum_zile_linieSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->date_add_acum_zile_interval_linieCheckBox->setChecked(true);
                    } else {
                        ui->date_add_acum_zile_interval_linieCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->date_add_acum_luni_linieCheckBox->objectName()) { // date_add_acum_luni_linie
                ui->date_add_acum_luni_linieCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->date_add_acum_luni_linieSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->date_add_acum_luni_interval_linieCheckBox->setChecked(true);
                    } else {
                        ui->date_add_acum_luni_interval_linieCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->date_add_intre_linieCheckBox->objectName()) { // date_add_intre_linie
                ui->date_add_intre_linieCheckBox->setChecked(true);
                vals.clear();
                vals  = g.split("|");
                if (vals.count() == 2) {
                    ui->date_add_min_linieDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(0),"yyyy-MM-dd  HH:mm:00"));
                    ui->date_add_max_linieDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(1),"yyyy-MM-dd  HH:mm:00"));
                }
            } else if (i == ui->date_edit_aziCheckBox->objectName()) { // date_edit_azi
                ui->date_edit_aziCheckBox->setChecked(true);
            } else if (i == ui->date_edit_lunaCheckBox->objectName()) { // date_edit_luna
                ui->date_edit_lunaCheckBox->setChecked(true);
            } else if (i == ui->date_edit_acum_zileCheckBox->objectName()) { // date_edit_acum_zile
                ui->date_edit_acum_zileCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->date_edit_acum_zileSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->date_edit_acum_zile_intervalCheckBox->setChecked(true);
                    } else {
                        ui->date_edit_acum_zile_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->date_edit_acum_luniCheckBox->objectName()) { // date_edit_aum_luni
                ui->date_edit_acum_luniCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->date_edit_acum_luniSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->date_edit_acum_luni_intervalCheckBox->setChecked(true);
                    } else {
                        ui->date_edit_acum_luni_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->date_edit_intreCheckBox->objectName()) { // date_edit_intre
                ui->date_edit_intreCheckBox->setChecked(true);
                vals.clear();
                vals  = g.split("|");
                if (vals.count() == 2) {
                    ui->date_edit_minDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(0),"yyyy-MM-dd  HH:mm:00"));
                    ui->date_edit_maxDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(1),"yyyy-MM-dd  HH:mm:00"));
                }
            } else if (i == ui->date_receptie_aziCheckBox->objectName()) { // date_receptie_azi
                ui->date_receptie_aziCheckBox->setChecked(true);
            } else if (i == ui->date_receptie_lunaCheckBox->objectName()) { // date_receptie_luna
                ui->date_receptie_lunaCheckBox->setChecked(true);
            } else if (i == ui->date_receptie_acum_zileCheckBox->objectName()) { // date_receptie_acum_zile
                ui->date_receptie_acum_zileCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->date_receptie_acum_zileSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->date_receptie_acum_zile_intervalCheckBox->setChecked(true);
                    } else {
                        ui->date_receptie_acum_zile_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->date_receptie_acum_luniCheckBox->objectName()) { // date_receptie_acum_luni
                ui->date_receptie_acum_luniCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->date_receptie_acum_luniSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->date_receptie_acum_luni_intervalCheckBox->setChecked(true);
                    } else {
                        ui->date_receptie_acum_luni_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->date_receptie_intreCheckBox->objectName()) { // date_receptie_intre
                ui->date_receptie_intreCheckBox->setChecked(true);
                vals.clear();
                vals  = g.split("|");
                if (vals.count() == 2) {
                    ui->date_receptie_minDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(0),"yyyy-MM-dd  HH:mm:00"));
                    ui->date_receptie_maxDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(1),"yyyy-MM-dd  HH:mm:00"));
                }
            } else if (i == ui->cantitate_eqCheckBox->objectName()) { // cantitate =
                ui->cantitate_eqCheckBox->setChecked(true);
                ui->cantitate_eqSpinBox->setValue(g.toInt());
            } else if (i == ui->cantitate_ltCheckBox->objectName()) { // cantitate <
                ui->cantitate_ltCheckBox->setChecked(true);
                ui->cantitate_ltSpinBox->setValue(g.toInt());
            } else if (i == ui->cantitate_gtCheckBox->objectName()) { // cantitate >
                ui->cantitate_gtCheckBox->setChecked(true);
                ui->cantitate_gtSpinBox->setValue(g.toInt());
            } else if (i == ui->cantitate_in_eqCheckBox->objectName()) { // cantitate_in =
                ui->cantitate_in_eqCheckBox->setChecked(true);
                ui->cantitate_in_eqSpinBox->setValue(g.toInt());
            } else if (i == ui->cantitate_in_gtCheckBox->objectName()) { // cantitate_in >
                ui->cantitate_in_gtCheckBox->setChecked(true);
                ui->cantitate_in_gtSpinBox->setValue(g.toInt());
            } else if (i == ui->cantitate_in_ltCheckBox->objectName()) { // cantitate in <
                ui->cantitate_in_ltCheckBox->setChecked(true);
                ui->cantitate_in_ltSpinBox->setValue(g.toInt());
            } else if (i == ui->termen_eqCheckBox->objectName()) { // termen =
                ui->termen_eqCheckBox->setChecked(true);
                ui->termen_eqSpinBox->setValue(g.toInt());
            } else if (i == ui->termen_gtCheckBox->objectName()) { // termen >
                ui->termen_gtCheckBox->setChecked(true);
                ui->termen_gtSpinBox->setValue(g.toInt());
            } else if (i == ui->termen_ltCheckBox->objectName()) { // termen <
                ui->termen_ltCheckBox->setChecked(true);
                ui->termen_ltSpinBox->setValue(g.toInt());
            } else if (i == ui->termen_sosireCheckBox->objectName()) { // termen_sosire
                ui->termen_sosireCheckBox->setChecked(true);
                ui->termen_sosireSpinBox->setValue(g.toInt());
            } else if (i == ui->termen_depasitCheckBox->objectName()) { // termen depasit
                ui->termen_depasitCheckBox->setChecked(true);
            } else if (i == ui->pret_a_eqCheckBox->objectName()) { // pret_a =
                ui->pret_a_eqCheckBox->setChecked(true);
                ui->pret_a_eqSpinBox->setValue(this->round(g.toDouble()));
            } else if (i == ui->pret_a_gtCheckBox->objectName()) { // pret_a >
                ui->pret_a_gtCheckBox->setChecked(true);
                ui->pret_a_gtSpinBox->setValue(this->round(g.toDouble()));
            } else if (i == ui->pret_a_ltCheckBox->objectName()) { // pret_a <
                ui->pret_a_ltCheckBox->setChecked(true);
                ui->pret_a_ltSpinBox->setValue(this->round(g.toDouble()));
            } else if (i == ui->pret_v_eqCheckBox->objectName()) { // pret_v =
                ui->pret_v_eqCheckBox->setChecked(true);
                ui->pret_v_eqSpinBox->setValue(this->round(g.toDouble()));
            } else if (i == ui->pret_v_gtCheckBox->objectName()) { // pret_v >
                ui->pret_v_gtCheckBox->setChecked(true);
                ui->pret_v_gtSpinBox->setValue(this->round(g.toDouble()));
            } else if (i == ui->pret_v_ltCheckBox->objectName()) { // pret_v <
                ui->pret_v_ltCheckBox->setChecked(true);
                ui->pret_v_ltSpinBox->setValue(this->round(g.toDouble()));
            } else if (i == ui->programare_aziCheckBox->objectName()) { // programare_azi
                ui->programare_aziCheckBox->setChecked(true);
            } else if (i == ui->programare_maineCheckBox->objectName()) { // programare_maine
                ui->programare_maineCheckBox->setChecked(true);
            } else if (i == ui->programare_lunaCheckBox->objectName()) { // programare_luna
                ui->programare_lunaCheckBox->setChecked(true);
            } else if (i == ui->programare_acum_zileCheckBox->objectName()) { // programare_acum_zile
                ui->programare_acum_zileCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->programare_acum_zileSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->programare_acum_zile_intervalCheckBox->setChecked(true);
                    } else {
                        ui->programare_acum_zile_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->programare_acum_luniCheckBox->objectName()) { // programare_acum_luni
                ui->programare_acum_luniCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->programare_acum_luniSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->programare_acum_luni_intervalCheckBox->setChecked(true);
                    } else {
                        ui->programare_acum_luni_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->programare_peste_zileCheckBox->objectName()) { // programare_peste_zile
                ui->programare_peste_zileCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->programare_peste_zileSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->programare_peste_zile_intervalCheckBox->setChecked(true);
                    } else {
                        ui->programare_peste_zile_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->programare_peste_luniCheckBox->objectName()) { // programare_peste_luni
                ui->programare_peste_luniCheckBox->setChecked(true);
                vals.clear();
                vals = g.split("|");
                if (vals.count() == 2) {
                    ui->programare_peste_luniSpinBox->setValue(vals.at(0).toInt());
                    if(vals.at(1) == "1") {
                        ui->programare_peste_luni_intervalCheckBox->setChecked(true);
                    } else {
                        ui->programare_peste_luni_intervalCheckBox->setChecked(false);
                    }
                }
            } else if (i == ui->programare_intreCheckBox->objectName()) { // programare_intre
                ui->programare_intreCheckBox->setChecked(true);
                vals.clear();
                vals  = g.split("|");
                if (vals.count() == 2) {
                    ui->programare_intre_minDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(0),"yyyy-MM-dd  HH:mm:00"));
                    ui->programare_intre_minDateTimeEdit->setDateTime(QDateTime::fromString(vals.at(1),"yyyy-MM-dd  HH:mm:00"));
                }
            } else if (i == ui->programare_faraCheckBox->objectName()) {
                ui->programare_faraCheckBox->setChecked(true);
            } else if (i == ui->contactatCheckBox->objectName()) { // contactat
                ui->contactatCheckBox->setChecked(true);
                ui->contactatComboBox->setCurrentIndex(g.toInt());
            }
        }
        // end active
    }
}

void AddRaport::resetPrivileges() {
    int nivel = The::nivel_user();

    ui->saveButton->setEnabled(false);

    if (nivel < 3) {
        ui->tabWidget->removeTab(3);
    }

    if (nivel > 3) {
        ui->saveButton->setEnabled(true);
    }
}
