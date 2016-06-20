#include "users.h"
#include "ui_users.h"

Users::Users(QWidget *parent) : QDialog(parent), ui(new Ui::Users) {
    ui->setupUi(this);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    this->addUserDlg = 0;
    this->editUserDlg = 0;
    this->editUserDlg = 0;

    ui->usersWidget->setColumnHidden(0,true);

    ui->usersWidget->setUniformRowHeights(true);
    ui->usersWidget->setRootIsDecorated(false);
    ui->usersWidget->setEditTriggers(QTreeWidget::NoEditTriggers);
    ui->usersWidget->setSelectionMode(QTreeWidget::SingleSelection);
    ui->usersWidget->setSelectionBehavior(QTreeWidget::SelectRows);
    ui->usersWidget->setFrameStyle(QFrame::Box | QFrame::Plain);
    ui->usersWidget->header()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->usersWidget->header()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->usersWidget->header()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->usersWidget->header()->setSectionResizeMode(4,QHeaderView::Stretch);
    ui->usersWidget->header()->setStretchLastSection(false);
    ui->usersWidget->header()->setDefaultAlignment(Qt::AlignCenter);

    ui->usersWidget->setSortingEnabled(true);
    ui->usersWidget->setDragDropMode(QTreeWidget::NoDragDrop);
    ui->usersWidget->setAlternatingRowColors(true);

    connect(ui->usersWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(editUser(QTreeWidgetItem*,int)));
    connect(ui->editButton,SIGNAL(clicked()),this,SLOT(editUserButton()));
    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addUser()));
    connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deleteUser()));
}

Users::~Users() {
    delete ui;
}

void Users::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
}

void Users::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
}

void Users::getUsers() {
    QUrlQuery query;

    query.addQueryItem("user_hash", The::user_hash());

    QUrl url = QUrl(The::server_url() + "users/get_users.json");
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseUsers(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Users::parseUsers(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Users","parseUsers();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Users","parseUsers();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        ui->usersWidget->setUpdatesEnabled(false);

        ui->usersWidget->clear();

        foreach (QVariant user, result["users"].toList()) {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setTextAlignment(0,Qt::AlignRight);
            item->setText(0, user.toMap()["id"].toString());
            item->setText(1, user.toMap()["username"].toString());
            item->setText(2, user.toMap()["nivel"].toString());
            item->setText(3, user.toMap()["nume"].toString());
            item->setText(4, user.toMap()["email"].toString());
            ui->usersWidget->addTopLevelItem(item);
        }
        ui->usersWidget->setUpdatesEnabled(true);
    }
}

void Users::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"Users","handleNetworkError();",Logger::Network);
}

void Users::editUser(QTreeWidgetItem *item, int col) {
    Q_UNUSED(col);
    if (item){
        if (!editUserDlg) {
            editUserDlg = new EditUser(this);
            connect(this->editUserDlg,SIGNAL(kill()),this,SLOT(beforeKill()));
        }
        editUserDlg->setUserId(item->text(0).toInt(), item->text(1), item->text(2).toInt(),item->text(3),item->text(4));
        if (editUserDlg->exec()) {
            this->getUsers();
        }
    }
}

void Users::editUserButton() {
    QList<QTreeWidgetItem*> items = ui->usersWidget->selectedItems();
    if (items.count()) {
        this->editUser(items.at(0));
    }
}

void Users::addUser() {
    if (!this->addUserDlg) {
        this->addUserDlg = new AddUser(this);
        connect(this->addUserDlg,SIGNAL(kill()),this,SLOT(beforeKill()));
    }
    if (this->addUserDlg->exec()) {
        this->getUsers();
    }
}

void Users::deleteUser() {
    QList<QTreeWidgetItem*> items = ui->usersWidget->selectedItems();

    if (items.count()) {
        QUrlQuery query;
        QTreeWidgetItem* item = items.at(0);

        if (QMessageBox::question(this, "Comfirmati Stergerea",
            QString("Sunteti sigur ca vreti sa stergeti userul '%1'").arg(
            item->text(1)),QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
        {
            query.addQueryItem("user_hash", The::user_hash());
            query.addQueryItem("user_id", item->text(0));

            QUrl url = QUrl(The::server_url() + "users/delete_user.json");
            QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

            connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
            connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
            loadingStart();
        }
    }
}

void Users::parseAnswer(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Users","parseAnswer();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Users","parseAnswer();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        this->getUsers();
    }
}

void Users::beforeKill() {
    emit kill();
    this->reject();
}
