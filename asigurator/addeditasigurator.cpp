#include "addeditasigurator.h"
#include "ui_addeditasigurator.h"

AddEditAsigurator::AddEditAsigurator(QWidget *parent) : QDialog(parent), ui(new Ui::AddEditAsigurator) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addEdit()));
    connect(ui->editButton,SIGNAL(clicked()),this,SLOT(addEdit()));
}

AddEditAsigurator::~AddEditAsigurator() {
    delete ui;
}

void AddEditAsigurator::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
}

void AddEditAsigurator::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
}

void AddEditAsigurator::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"AddEditAsigurator","handleNetworkError();",Logger::Network);
}

void AddEditAsigurator::setAddMode() {
    this->add = true;
    ui->asiguratorLineEdit->clear();
    ui->asiguratorComboBox->setCurrentIndex(1);
    this->asigurator_id = 0;
    ui->addButton->setHidden(false);
    ui->editButton->setHidden(true);
}

void AddEditAsigurator::setEditMode(QString asigurator, int tip, int asigurator_id) {
    this->add = false;
    ui->asiguratorLineEdit->setText(asigurator);
    this->asigurator_id = asigurator_id;
    if (tip > 0) {
        ui->asiguratorComboBox->setCurrentIndex(tip -1);
    }
    ui->addButton->setHidden(true);
    ui->editButton->setHidden(false);
}


void AddEditAsigurator::addEdit() {
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());

    if (this->asigurator_id) {
        query.addQueryItem("asigurator_id",QString("%1").arg(this->asigurator_id));
    }

    if (!ui->asiguratorLineEdit->text().isEmpty()){
        query.addQueryItem("asigurator",ui->asiguratorLineEdit->text().toUpper().trimmed());
    } else {
        QMessageBox::warning(this,"Eroare","Complectati numele asiguratorului!");
        return;
    }

    query.addQueryItem("tip",QString("%1").arg(ui->asiguratorComboBox->currentIndex() + 1));

    QUrl url;
    if (this->add) {
        url = QUrl(QString(The::server_url() + "asiguratori/add_asigurator.json"));
    } else {
        url = QUrl(QString(The::server_url() + "asiguratori/edit_asigurator.json"));
    }

    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void AddEditAsigurator::parseAnswer(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","AddEditAsigurator","parseAnswer();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"AddEditAsigurator","parseAnswer();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        this->done(1);
    }
}


