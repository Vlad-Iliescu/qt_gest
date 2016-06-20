#include "furnizori.h"
#include "ui_furnizori.h"

Furnizori::Furnizori(QWidget *parent) : QDialog(parent), ui(new Ui::Furnizori) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    ui->treeWidget->setColumnHidden(0,true);
    connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deleteFurnizor()));

    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addFunizor()));
    connect(ui->editButton,SIGNAL(clicked()),this,SLOT(editFurnizor()));
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(itemDC(QTreeWidgetItem*,int)));

    this->furnizor = 0;
}

Furnizori::~Furnizori() {
    delete ui;
}

void Furnizori::beforeKill() {
    emit kill();
    this->reject();
}

void Furnizori::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
    ui->addButton->setEnabled(true);
    ui->editButton->setEnabled(true);
}

void Furnizori::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
    ui->addButton->setEnabled(false);
    ui->editButton->setEnabled(false);
}

void Furnizori::getFurnizori() {
    QUrlQuery query;

    query.addQueryItem("user_hash", The::user_hash());

    QUrl url = QUrl(QString(The::server_url() + "furnizori/get_furnizori.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseFurnizori(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Furnizori::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"Furnizori","handleNetworkError();",Logger::Network);
}

void Furnizori::parseFurnizori(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Furnizori","parseFurnizori();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Furnizori","parseFurnizori();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        ui->treeWidget->clear();
        QTreeWidgetItem *item;
        foreach(QVariant furnizor,result["furnizori"].toList()) {
            item = new QTreeWidgetItem();
            item->setText(0,furnizor.toMap()["id"].toString());
            item->setText(1,furnizor.toMap()["denumire"].toString());
            ui->treeWidget->addTopLevelItem(item);
        }
    }
}

void Furnizori::parseDelete(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Furnizori","parseDelete();",Logger::JSON);
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
        this->getFurnizori();
    }
}

void Furnizori::addFunizor() {
    if (!this->furnizor) {
        this->furnizor = new AddEditFrunizor(this);

        connect(this->furnizor,SIGNAL(kill()),this,SLOT(beforeKill()));
    }

    this->furnizor->setAddMode();
    if (this->furnizor->exec()) {
        this->getFurnizori();
    }
}

void Furnizori::editFurnizor() {
    QList< QTreeWidgetItem * > items = ui->treeWidget->selectedItems();

    if (!items.count()) {
        return;
    }

    if (!this->furnizor) {
        this->furnizor = new AddEditFrunizor(this);

        connect(this->furnizor,SIGNAL(kill()),this,SLOT(beforeKill()));
    }

    this->furnizor->setEditMode(items.at(0)->text(1),items.at(0)->text(0).toInt());
    if (this->furnizor->exec()) {
        this->getFurnizori();
    }
}

void Furnizori::itemDC(QTreeWidgetItem *item, int col) {
    Q_UNUSED(item)
    Q_UNUSED(col)
    this->editFurnizor();
}

void Furnizori::deleteFurnizor() {
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
        query.addQueryItem("furnizor_id", items.at(0)->text(0));

        QUrl url = QUrl(QString(The::server_url() + "furnizori/sterge_furnizor.json"));

        QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

        connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseDelete(QByteArray)));
        connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
        loadingStart();
    }
}
