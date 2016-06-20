#include "addeditfrunizor.h"
#include "ui_addeditfrunizor.h"

AddEditFrunizor::AddEditFrunizor(QWidget *parent) : QDialog(parent),  ui(new Ui::AddEditFrunizor) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addEdit()));
    connect(ui->editButton,SIGNAL(clicked()),this,SLOT(addEdit()));
}

void AddEditFrunizor::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
}

void AddEditFrunizor::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
}

void AddEditFrunizor::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"AddEditFrunizor","handleNetworkError();",Logger::Network);
}

AddEditFrunizor::~AddEditFrunizor() {
    delete ui;
}

void AddEditFrunizor::setAddMode() {
    this->add = true;
    ui->furnizorLineEdit->clear();
    this->furnizor_id = 0;
    ui->addButton->setHidden(false);
    ui->editButton->setHidden(true);
}

void AddEditFrunizor::setEditMode(QString furnizor, int furnizor_id) {
    this->add = false;
    ui->furnizorLineEdit->setText(furnizor);
    this->furnizor_id = furnizor_id;
    ui->addButton->setHidden(true);
    ui->editButton->setHidden(false);
}

void AddEditFrunizor::addEdit() {
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());

    if (this->furnizor_id) {
        query.addQueryItem("furnizor_id",QString("%1").arg(this->furnizor_id));
    }

    if (!ui->furnizorLineEdit->text().isEmpty()){
        query.addQueryItem("furnizor",ui->furnizorLineEdit->text().toUpper());
    } else {
        QMessageBox::warning(this,"Eroare","Complectati numele furnizorului!");
        return;
    }

    QUrl url;
    if (this->add) {
        url = QUrl(QString(The::server_url() + "furnizori/add_furnizor.json"));
    } else {
        url = QUrl(QString(The::server_url() + "furnizori/edit_furnizor.json"));
    }

    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void AddEditFrunizor::parseAnswer(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","AddEditFrunizor","parseAnswer();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"AddEditFrunizor","parseAnswer();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        this->done(1);
    }
}
