#include "rapoarte.h"
#include "ui_rapoarte.h"

Rapoarte::Rapoarte(QWidget *parent) : QDialog(parent), ui(new Ui::Rapoarte) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

//    this->raport = new Raport(this);
    this->addraport = 0;

    ui->treeWidget->setColumnHidden(0,true);
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(getRaport(QTreeWidgetItem*,int)));
    connect(ui->viewButton,SIGNAL(clicked()),this,SLOT(getRaport()));
    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addNew()));
    connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(stergereRaport()));
    connect(ui->editButton,SIGNAL(clicked()),this,SLOT(editRaport()));

}

Rapoarte::~Rapoarte() {
    delete ui;
}

void Rapoarte::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
}

void Rapoarte::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
}

void Rapoarte::getRapoarte() {
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());

    QUrl url = QUrl(QString(The::server_url() + "rapoarte/get_rapoarte.json"));
    QObject *scan = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(scan, SIGNAL(data(QByteArray)),this, SLOT(parseRapoarte(QByteArray)));
    connect(scan, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Rapoarte::parseRapoarte(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Rapoarte","parseRapoarte();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Rapoarte","parseRapoarte();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        QTreeWidgetItem *item;

        ui->treeWidget->clear();
        ui->treeWidget->setUpdatesEnabled(false);
        foreach(const QVariant &raport,result["rapoarte"].toList()) {
            item = new QTreeWidgetItem();
            item->setText(0,raport.toMap()["id"].toString());
            item->setText(1,raport.toMap()["denumire"].toString());
            ui->treeWidget->addTopLevelItem(item);
        }
        ui->treeWidget->setUpdatesEnabled(true);
    }
}

void Rapoarte::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"Rapoarte","handleNetworkError();",Logger::Network);
}

void Rapoarte::getRaport(QTreeWidgetItem *item, int col) {
    Q_UNUSED(col);

    if (!item) {
        QList< QTreeWidgetItem* > items = ui->treeWidget->selectedItems();
        if (items.count()) {
            item = items.at(0);
        }
    }

    if (!item) return;

    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());
    query.addQueryItem("raport_id",item->text(0));

    QUrl url = QUrl(QString(The::server_url() + "rapoarte/generate_raport.json"));
    QObject *scan = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(scan, SIGNAL(data(QByteArray)),this, SLOT(parseRaport(QByteArray)));
    connect(scan, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Rapoarte::queryResult(QString SQLQuery) {
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());
    query.addQueryItem("query", SQLQuery);

    QUrl url = QUrl(QString(The::server_url() + "raport/get_raport_query.json"));
    QObject *scan = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8().replace("+","%2B"));

    connect(scan, SIGNAL(data(QByteArray)),this, SLOT(parseRaport(QByteArray)));
    connect(scan, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Rapoarte::parseRaport(QByteArray data) {
    loadingStop();
//    qDebug() << data;

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Rapoarte","parseRaport();",Logger::JSON);
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
        emit raportReady(result);
    }
}

void Rapoarte::addNew() {
    this->addraport = new AddRaport(this);
    connect(this->addraport,SIGNAL(rapoarteChanged()),this,SLOT(getRapoarte()));
    connect(this->addraport,SIGNAL(query(QByteArray)),this,SLOT(viewRaport(QByteArray)));
    connect(this->addraport,SIGNAL(kill()),this,SLOT(beforeKill()));
    this->addraport->setModal(false);

    this->addraport->show();
    this->getRapoarte();
}

void Rapoarte::stergereRaport() {
    QTreeWidgetItem *item = 0;
    QUrlQuery query;

    QList< QTreeWidgetItem* > items = ui->treeWidget->selectedItems();
    if (items.count()) {
        item = items.at(0);
    } else {
        return;
    }

    if (QMessageBox::question(this, "Comfirmati Stergerea",
                                   QString("Sunteti sigur ca vreti sa stergeti raportul '%1'")
                                   .arg(items.at(0)->text(1)),QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) {
        return;
    }


    query.addQueryItem("user_hash",The::user_hash());
    query.addQueryItem("raport_id", item->text(0));

    QUrl url = QUrl(QString(The::server_url() + "rapoarte/sterge_raport.json"));
    QObject *scan = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(scan, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(scan, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Rapoarte::parseAnswer(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data, &ok).toMap();

    if(!ok) {
        QMessageBox::warning(this, "Eroare parsare", "A aparut o eroare!");
        emit error("Comenzi : Error parsing json! {function parseAnswer(); }");
        return;
    }

    bool status = result["status"].toBool();
    QString err = result["error"].toString();
    if (status) {
        this->getRapoarte();
    } else {
        QMessageBox::warning(this, "Eroare server", err);
        emit error("Contracte : Eroare server! {function parseAnswer(); } " + err);
        if (err == "Sesiunea a expirat!") {
            emit kill();
            this->done(0);
        }
    }
}

void Rapoarte::editRaport() {
    QTreeWidgetItem *item = 0;

    QList< QTreeWidgetItem* > items = ui->treeWidget->selectedItems();
    if (items.count()) {
        item = items.at(0);
    } else {
        return;
    }

    this->addraport = new AddRaport(this);
    connect(this->addraport,SIGNAL(rapoarteChanged()),this,SLOT(getRapoarte()));
    connect(this->addraport,SIGNAL(query(QByteArray)),this,SLOT(viewRaport(QByteArray)));
    connect(this->addraport,SIGNAL(kill()),this,SLOT(beforeKill()));

    this->addraport->setModal(false);
    this->addraport->getEditData(item->text(0).toInt());
    this->addraport->show();
}

void Rapoarte::viewRaport(QByteArray data) {

    QUrl url = QUrl(QString(The::server_url() + "rapoarte/generate_raport_from_query.json"));
    QObject *scan = The::http()->post(url, data);

    connect(scan, SIGNAL(data(QByteArray)),this, SLOT(parseRaport(QByteArray)));
    connect(scan, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Rapoarte::beforeKill() {
    emit kill();
    this->reject();
}

void Rapoarte::resetPrivileges() {
    int nivel = The::nivel_user();

    ui->addButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);

    if (nivel > 3) {
        ui->addButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
    }
}
