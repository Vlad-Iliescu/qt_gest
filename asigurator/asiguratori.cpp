#include "asiguratori.h"
#include "ui_asiguratori.h"

Asiguratori::Asiguratori(QWidget *parent) : QDialog(parent), ui(new Ui::Asiguratori) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    ui->treeWidget->setColumnHidden(0,true);
    connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deleteAsigurator()));

    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addAsigurator()));
    connect(ui->editButton,SIGNAL(clicked()),this,SLOT(editAsigurator()));
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(itemDC(QTreeWidgetItem*,int)));

    this->asigurator = 0;
}

Asiguratori::~Asiguratori() {
    delete ui;
}

void Asiguratori::beforeKill() {
    emit kill();
    this->reject();
}

void Asiguratori::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
    ui->addButton->setEnabled(true);
    ui->editButton->setEnabled(true);
}

void Asiguratori::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
    ui->addButton->setEnabled(false);
    ui->editButton->setEnabled(false);
}

void Asiguratori::getAsiguratori() {
    QUrlQuery query;

    query.addQueryItem("user_hash", The::user_hash());

    QUrl url = QUrl(QString(The::server_url() + "asiguratori/get_asiguratori.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAsiguratori(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Asiguratori::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"Asiguratori","handleNetworkError();",Logger::Network);
}

void Asiguratori::parseAsiguratori(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Asiguratori","parseAsiguratori();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Asiguratori","parseAsiguratori();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        ui->treeWidget->clear();
        QTreeWidgetItem *item;
        foreach(QVariant furnizor,result["asiguratori"].toList()) {
            item = new QTreeWidgetItem();
            item->setText(0,furnizor.toMap()["id"].toString());
            item->setText(1,furnizor.toMap()["denumire"].toString());
            item->setText(2,(furnizor.toMap()["tip"].toInt() == 1)?"1 - Gestiune":"2 - Non Gestiune");
            item->setData(2, Qt::UserRole, furnizor.toMap()["tip"]);
            ui->treeWidget->addTopLevelItem(item);
        }
    }
}

void Asiguratori::parseDelete(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Asiguratori","parseDelete();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Furnizori","parseDelete();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        this->getAsiguratori();
    }
}

void Asiguratori::addAsigurator() {
    if (!this->asigurator) {
        this->asigurator = new AddEditAsigurator(this);

        connect(this->asigurator,SIGNAL(kill()),this,SLOT(beforeKill()));
    }

    this->asigurator->setAddMode();
    if (this->asigurator->exec()) {
        this->getAsiguratori();
    }
}

void Asiguratori::editAsigurator() {
    QList< QTreeWidgetItem * > items = ui->treeWidget->selectedItems();

    if (!items.count()) {
        return;
    }

    if (!this->asigurator) {
        this->asigurator = new AddEditAsigurator(this);

        connect(this->asigurator,SIGNAL(kill()),this,SLOT(beforeKill()));
    }

    this->asigurator->setEditMode(items.at(0)->text(1), items.at(0)->data(2,Qt::UserRole).toInt(), items.at(0)->text(0).toInt());
    if (this->asigurator->exec()) {
        this->getAsiguratori();
    }
}

void Asiguratori::itemDC(QTreeWidgetItem *item, int col) {
    Q_UNUSED(item)
    Q_UNUSED(col)
    this->editAsigurator();
}

void Asiguratori::deleteAsigurator() {
    QList< QTreeWidgetItem * > items = ui->treeWidget->selectedItems();

    if (!items.count()){
        return;
    }

    if (QMessageBox::question(this, "Comfirmati Stergerea",
                QString("Sunteti sigur ca vreti sa stergeti furnizorul '%1'").arg(
                items.at(0)->text(1)),QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
            {
        QUrlQuery query;

        query.addQueryItem("user_hash",The::user_hash());
        query.addQueryItem("asigurator_id", items.at(0)->text(0));

        QUrl url = QUrl(QString(The::server_url() + "asiguratori/sterge_asigurator.json"));

        QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

        connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseDelete(QByteArray)));
        connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
        loadingStart();
    }
}
