#include "addcontract.h"
#include "ui_addcontract.h"

AddContract::AddContract(QWidget *parent) : QDialog(parent), ui(new Ui::AddContract) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    this->getConsilieri();

    ui->programareDateTimeEdit->setDate(QDate::currentDate());

    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(addNewContract()));
}

AddContract::~AddContract() {
    delete ui;
}

void AddContract::getConsilieri() {
    QUrlQuery query;

    query.addQueryItem("user_hash", The::user_hash());

    QUrl url =  QUrl(QString(The::server_url() + "user/get_consilieri.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseConsilieri(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void AddContract::parseConsilieri(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","AddContract","parseConsilieri();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"AddContract","parseConsilieri();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        ui->consilierComboBox->clear();
        consilieri.clear();
        foreach(QVariant cons, result["consilieri"].toList()) {
            this->consilieri.insert(cons.toMap()["nume"].toString(), cons.toMap()["id"].toInt());
            ui->consilierComboBox->addItem(cons.toMap()["nume"].toString(), cons.toMap()["id"]);
        }
    }
}

void AddContract::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
    ui->saveButton->setEnabled(false);
}

void AddContract::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
    ui->saveButton->setEnabled(true);
}

void AddContract::addNewContract() {
    QUrlQuery query;
    QStringList errors;

    query.addQueryItem("user_hash", The::user_hash());

    if (ui->contractLineEdit->text().isEmpty()) {
        errors << "Completati campul contract!";
    } else {
        query.addQueryItem("contract",ui->contractLineEdit->text().trimmed().toUpper());
    }

    if (ui->autoLineEdit->text().isEmpty()) {
        errors << "Completati campul Auto!";
    } else {
        query.addQueryItem("auto",ui->autoLineEdit->text().trimmed().toUpper());
    }

    if (ui->contactatCheckBox->isChecked()) {
        query.addQueryItem("contactat","1");
    } else {
        query.addQueryItem("contactat","0");
    }

    if (ui->programareCheckBox->isChecked()) {
        query.addQueryItem("programare",ui->programareDateTimeEdit->text());
    }

    query.addQueryItem("numar_auto",ui->nrAutoLineEdit->text().trimmed().toUpper());
    query.addQueryItem("observatii",ui->observatiiTextEdit->toPlainText().trimmed());
    query.addQueryItem("consilier_id", ui->consilierComboBox->itemData(ui->consilierComboBox->currentIndex()).toString());

    if (errors.count()) {
        QMessageBox::warning(this,"Aveti Erori",errors.join("\n"));
        return;
    }

    QUrl url =  QUrl(QString(The::server_url() + "contracte/add_contract.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void AddContract::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"AddContract","handleNetworkError();",Logger::Network);
}

void AddContract::parseAnswer(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","AddContract","parseAnswer();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"AddContract","parseAnswer();",Logger::Server);
        if (status == 401) {
            emit kill();
            this->done(0);
        } else  if (status == 409) {
            int contract_id = result["contract_id"].toInt(&ok);
            if (ok && QMessageBox::question(this,"Editati","Editati",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
                emit added(contract_id);
                this->done(contract_id);
            }
        }
        return;
    } else {
        int contract_id = result["contract_id"].toInt(&ok);
        if (ok) {
            emit added(contract_id);
            this->done(contract_id);
        }
    }
}
