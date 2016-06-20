#include "edituser.h"
#include "ui_edituser.h"

EditUser::EditUser(QWidget *parent) : QDialog(parent), ui(new Ui::EditUser) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    connect(ui->okButton,SIGNAL(clicked()),this,SLOT(editUser()));
}

EditUser::~EditUser() {
    delete ui;
}

void EditUser::setUserId(int id, QString username, int nivel, QString name, QString email) {
    this->user_id = id;
    ui->usernameLineEdit->setText(username);
    ui->nivelComboBox->setCurrentIndex(nivel);
    ui->numeLineEdit->setText(name);
    ui->emailLineEdit->setText(email);
}

void EditUser::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
    ui->okButton->setEnabled(true);
}

void EditUser::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
    ui->okButton->setEnabled(false);
}

void EditUser::parseAnswer(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","EditUser","parseAnswer();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"EditUser","parseAnswer();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        this->done(1);
        return;
    }
}

void EditUser::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"AddContract","handleNetworkError();",Logger::Network);
}

void EditUser::editUser() {
    QUrlQuery query;
    QStringList errors;

    if (ui->usernameLineEdit->text().length() < 4) {
        errors << "Username trebuie sa fie de minim 4 caractere!";
    } else {
        query.addQueryItem("username",ui->usernameLineEdit->text());
    }

    if (ui->passLineEdit->text().length() < 4 && ui->passLineEdit->text().length() > 1) {
        errors << "Parola trebuie sa fie de minim 4 caractere!";
    } else {
        QCryptographicHash hasher(QCryptographicHash::Md5);
        hasher.addData(QByteArray().append(ui->passLineEdit->text()));
        query.addQueryItem("password",hasher.result().toHex());
    }

    query.addQueryItem("user_id",QString("%1").arg(this->user_id));
    query.addQueryItem("nivel",QString("%1").arg(ui->nivelComboBox->currentIndex()));
    query.addQueryItem("nume",ui->numeLineEdit->text().trimmed());
    query.addQueryItem("email",ui->emailLineEdit->text().trimmed());
    query.addQueryItem("user_hash", The::user_hash());

    if (errors.count()) {
        QMessageBox::warning(this, "Aveti erori in formular",errors.join("\n"));
        return;
    }

    QUrl url = QUrl(The::server_url() + "users/edit_user.json");
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}
