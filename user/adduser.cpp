#include "adduser.h"
#include "ui_adduser.h"

AddUser::AddUser(QWidget *parent) : QDialog(parent), ui(new Ui::AddUser) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    connect(ui->okButton,SIGNAL(clicked()),this,SLOT(addUser()));
}

AddUser::~AddUser() {
    delete ui;
}

void AddUser::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
    ui->okButton->setEnabled(true);
}

void AddUser::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
    ui->okButton->setEnabled(false);
}

void AddUser::parseAnswer(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","AddUser","parseAnswer();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"AddUser","parseAnswer();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        ui->usernameLineEdit->clear();
        ui->numeLineEdit->clear();
        ui->passLineEdit->clear();
        ui->nivelComboBox->setCurrentIndex(0);
        ui->emailLineEdit->clear();
        this->done(1);
    }
}

void AddUser::handleNetworkError(QNetworkReply *reply) {
     Log::logger()->logError(reply->errorString(),"AddContract","handleNetworkError();",Logger::Network);
}

void AddUser::addUser() {
    QUrlQuery query;
    QStringList errors;

    if (ui->usernameLineEdit->text().length() < 4) {
        errors << "Username trebuie sa fie de minim 4 caractere!";
    } else {
        query.addQueryItem("username",ui->usernameLineEdit->text().trimmed());
    }

    if (ui->passLineEdit->text().length() < 4) {
        errors << "Parola trebuie sa fie de minim 4 caractere!";
    } else {
        QCryptographicHash hasher(QCryptographicHash::Md5);
        hasher.addData(QByteArray().append(ui->passLineEdit->text()));
        query.addQueryItem("password",hasher.result().toHex());
    }

    query.addQueryItem("nivel", QString("%1").arg(ui->nivelComboBox->currentIndex()));
    query.addQueryItem("nume", ui->numeLineEdit->text().trimmed());
    query.addQueryItem("email",ui->emailLineEdit->text().trimmed());
    query.addQueryItem("user_hash", The::user_hash());

    if (errors.count()) {
        QMessageBox::warning(this, "Aveti erori in formular",errors.join("\n"));
        return;
    }

    QUrl url = QUrl(The::server_url() + "users/add_user.json");
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}
