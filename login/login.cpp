#include <QMessageBox>
#include <QUrlQuery>

#include "login.h"
#include "ui_login.h"

#include "global.h"
#include "log.h"

Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login) {
    ui->setupUi(this);
    this->nivel = 0;
    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

//    ui->usernameLineEdit->setText("rokordev");
//    ui->passwordLineEdit->setText("thereisnospoon");

    connect(ui->loginButton,SIGNAL(clicked()),this,SLOT(logare()));

    QUrl url = QUrl(QString(The::server_url() + "default/echo.json"));
    QObject *echo = The::http()->get(url);
    loadingStart();
    ui->quitButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    connect(echo, SIGNAL(data(QByteArray)),this, SLOT(loadingStop()));
    connect(echo, SIGNAL(error(QNetworkReply*)), this, SLOT(parseEcho(QNetworkReply*)));

}

Login::~Login() {
    delete ui;
}

void Login::loadingStop() {
    this->movie->stop();
    ui->loginButton->setEnabled(true);
    ui->loadingLabel->hide();
}

void Login::loadingStart() {
    this->movie->start();
    ui->loginButton->setEnabled(false);
    ui->loadingLabel->show();
}

int Login::getNivel() {
    return this->nivel;
}

QString Login::getUsername() {
    return this->username;
}

void Login::logare() {
    QUrlQuery query;
    QStringList errors;

    if (ui->usernameLineEdit->text().isEmpty()) {
        errors << "Completati userul!";
    } else {
        query.addQueryItem("username",ui->usernameLineEdit->text().trimmed());
    }

    if (ui->passwordLineEdit->text().isEmpty()) {
        errors << "Completati parola!";
    } else {
        QCryptographicHash hasher(QCryptographicHash::Md5);
        hasher.addData(QByteArray().append(ui->passwordLineEdit->text()));
        query.addQueryItem(QByteArray("password"),hasher.result().toHex());;
    }

    if (errors.count()) {
        QMessageBox::warning(this, "Eroare Login",errors.join("\n"));
        return;
    }

    QUrl url = QUrl(QString(The::server_url() + "user/logare.json"));
    QObject *log = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(log, SIGNAL(data(QByteArray)),this, SLOT(parseLogin(QByteArray)));
    connect(log, SIGNAL(error(QNetworkReply*)), this, SLOT(parseError(QNetworkReply*)));
    loadingStart();
}

void Login::parseError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"Login","parseError();",Logger::Network);
}

void Login::parseEcho(QNetworkReply *reply) {
    QMessageBox::warning(this,"Eroare Server",reply->errorString());
    Log::logger()->logError(reply->errorString(),"Login","parseEcho();",Logger::Network);
    this->done(0);
}

void Login::parseLogin(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data, &ok).toMap();

    if(!ok) {
        QMessageBox::warning(this, "Eroare parsare", "A aparut o eroare!");
        Log::logger()->logError("Error parsing JSON","Login","parseLogin();",Logger::JSON);
        return;
    }

    bool status = result["status"].toBool();
    QString err = result["error"].toString();

    if (status) {
        this->nivel = result["nivel"].toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Eroare server", "User Interzis!");
            Log::logger()->logError("User Interzis","Login","parseLogin();",Logger::Other);
            return;
        }
        this->username = result["nume"].toString();

        The::set_user_hash(result["login_hash"].toString());
        The::set_user_nivel(this->nivel);
        The::set_username(this->username);

        this->done(1);
        return;
    } else {
        QMessageBox::warning(this, "Eroare server", err);
        Log::logger()->logError("Server Error","Login","parseLogin();",Logger::Server);
        return;
    }
}
