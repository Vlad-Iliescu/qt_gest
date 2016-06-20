#include "mainmdi.h"
#include "ui_mainmdi.h"

MainMdi::MainMdi(QWidget *parent) : QWidget(parent), ui(new Ui::MainMdi) {
    ui->setupUi(this);

    this->createActions();

    ui->treeWidget->setColumnHidden(1,true);
    ui->treeWidget->setSortingEnabled(true);

    ui->treeWidget->setAutoFillBackground(true);

    ui->treeWidget->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(3,QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(4,QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(5,QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(6,QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(7,QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(8,QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(9,QHeaderView::ResizeToContents);

    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showCustomContextMenu(QPoint)));
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(editeazaContract(QTreeWidgetItem*,int)));
}

MainMdi::~MainMdi() {
    delete ui;
}

void MainMdi::closeEvent(QCloseEvent *event) {
    event->ignore();
}

void MainMdi::getContent() {
    QUrlQuery query;

    query.addQueryItem("user_hash", The::user_hash());

    QUrl url =  QUrl(QString(The::server_url() + "contracte/get_contracte.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseContent(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    emit loadingStart();
}

void MainMdi::parseContent(QByteArray data) {
    emit loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","MainMdi","parseContent();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"MainMdi","parseContent();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        double TVA = result["TVA"].toDouble(&ok);
        if (ok) {
            TVA = this->round(TVA/100);
            emit tva(TVA);
        } else {
            TVA = 0.24;
        }

        this->users.clear();

        foreach(const QVariant &user, result["users"].toList()) {
            this->users.insert(user.toMap()["id"].toInt(),user.toMap()["nume"].toString());
        }

        ui->treeWidget->clear();
        ui->treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        QTreeWidgetItem *item;
        QVariant temp;
        QDateTime date;

        ui->treeWidget->setUpdatesEnabled(false);

        foreach(const QVariant &contract, result["contracte"].toList()) {
            item = new QTreeWidgetItem();

            temp =  contract.toMap()["contract"];
            item->setText(0,temp.toString());

            temp = contract.toMap()["id"];
            item->setText(1,temp.toString());

            temp = contract.toMap()["auto"];
            item->setText(2,temp.toString());

            temp = contract.toMap()["numar_auto"];
            item->setText(3,temp.toString());

            date = contract.toMap()["date_added"].toDateTime();
            item->setText(4,date.toString("dd-MM-yyyy HH:mm"));
            item->setTextAlignment(4,Qt::AlignRight);

            temp = contract.toMap()["user_id"];
            item->setText(5,this->users.value(temp.toInt(),"NULL"));

            date = contract.toMap()["programare"].toDateTime();
            item->setText(6,date.toString("dd-MM-yyyy HH:mm"));
            item->setTextAlignment(6,Qt::AlignRight);

            temp = contract.toMap()["contactat"];
            item->setText(7,(temp.toString() == "T")?"Da":"Nu");
            item->setTextAlignment(7,Qt::AlignRight);

            temp = contract.toMap()["total"];
            item->setText(8,QString::number(this->round(temp.toDouble()),'f',2));
            item->setTextAlignment(8,Qt::AlignRight);

            temp = contract.toMap()["observatii"];
            item->setText(9,temp.toString());

            ui->treeWidget->addTopLevelItem(item);
        }
        ui->treeWidget->setUpdatesEnabled(true);
    }
}

void MainMdi::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"AddContract","handleNetworkError();",Logger::Network);
}

void MainMdi::showCustomContextMenu(const QPoint point) {
    this->menu.clear();

    QTreeWidgetItem *item = ui->treeWidget->itemAt(point);

    if (item) {
        if (!item->parent()) {
            this->menu.addAction(this->editeaza);
            this->menu.addAction(this->finalizare);
        }
    }

    this->menu.exec(QCursor::pos());
}

void MainMdi::createActions() {
    this->editeaza = new QAction(this);
    this->editeaza->setIcon(QIcon(":/edit"));
    this->editeaza->setText("&Edit");
    connect(this->editeaza,SIGNAL(triggered()),this,SLOT(editeazaContract()));

    this->finalizare = new QAction(this);
    this->finalizare->setIcon(QIcon(":/finalizare"));
    this->finalizare->setText("&Finalizare");
    connect(this->finalizare,SIGNAL(triggered()),this,SLOT(finalizeazaContract()));
}

void MainMdi::editeazaContract(QTreeWidgetItem *item, int col) {
    if (!this->editeaza->isEnabled()) {
        return;
    }

    Q_UNUSED(col);

    if (!item) {
        item = ui->treeWidget->currentItem();
        if (!item) {
            return;
        }
    }
    bool ok;
    int contract_id = item->text(1).toInt(&ok);

    if (ok) {
        emit editContract(contract_id);
    }
}

void MainMdi::editeazaContract(int contract_id) {
    if (!this->editeaza->isEnabled()) {
        return;
    }
    emit editContract(contract_id);
}

void MainMdi::finalizeazaContract() {
    if (!this->finalizare->isEnabled()) {
        return;
    }

    QTreeWidgetItem *item = ui->treeWidget->currentItem();

    if (!item) {
        return;
    }

    if (item->parent()) {
        item = item->parent();
    }

    QMessageBox *msg = new QMessageBox(this);

    QAbstractButton *facturat = msg->addButton("Facturat",QMessageBox::AcceptRole);
    QAbstractButton *renuntat = msg->addButton("Renuntat",QMessageBox::AcceptRole);
    QAbstractButton *cancel = msg->addButton("Cancel",QMessageBox::RejectRole);

    msg->setWindowTitle("Confirmare");
    msg->setIcon(QMessageBox::Question);
    msg->setWindowModality(Qt::WindowModal);
    msg->setText(QString("Finalizarea contractului '%1' presupune inchiderea fiecareri linii.\n")
                 .arg(item->text(0)) +
                 "Continuati?");
    msg->setDefaultButton(qobject_cast<QPushButton*>(cancel));

    msg->exec();
    QString result = "";

    if (msg->clickedButton() == cancel) {
        return;
    } else if (msg->clickedButton() == facturat) {
        if (QMessageBox::question(this,"Confirmare","Sunteti Sigur!?"
                                        "Continuati?",
                                        QMessageBox::Yes,QMessageBox::No) == QMessageBox::No) {
           return;
        }
        result = "Facturat";
    } else if (msg->clickedButton() == renuntat) {
        if (QMessageBox::question(this,"Confirmare","Sunteti Sigur!?"
                                        "Continuati?",
                                        QMessageBox::Yes,QMessageBox::No) == QMessageBox::No) {
           return;
        }
        result = "Renuntat";
    }

    QUrlQuery query;

    query.addQueryItem("user_hash", The::user_hash());
    query.addQueryItem("contract_id",item->text(1));
    query.addQueryItem("motiv",result);

    QUrl url =  QUrl(QString(The::server_url() + "contracte/finalizare.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    emit loadingStart();
}

void MainMdi::finalizeazaContract(int contract_id) {
    if (!this->finalizare->isEnabled()) {
        return;
    }

    QMessageBox *msg = new QMessageBox(this);

    QAbstractButton *facturat = msg->addButton("Facturat",QMessageBox::AcceptRole);
    QAbstractButton *renuntat = msg->addButton("Renuntat",QMessageBox::AcceptRole);
    QAbstractButton *cancel = msg->addButton("Cancel",QMessageBox::RejectRole);

    msg->setWindowTitle("Confirmare");
    msg->setIcon(QMessageBox::Question);
    msg->setWindowModality(Qt::WindowModal);
    msg->setText("Finalizarea contractului presupune inchiderea fiecareri linii.\n"
                 "Continuati?");
    msg->setDefaultButton(qobject_cast<QPushButton*>(cancel));

    msg->exec();
    QString result = "";

    if (msg->clickedButton() == cancel) {
        return;
    } else if (msg->clickedButton() == facturat) {
        if (QMessageBox::question(this,"Confirmare","Sunteti Sigur!?"
                                        "Continuati?",
                                        QMessageBox::Yes,QMessageBox::No) == QMessageBox::No) {
           return;
        }
        result = "Facturat";
    } else if (msg->clickedButton() == renuntat) {
        if (QMessageBox::question(this,"Confirmare","Sunteti Sigur!?"
                                        "Continuati?",
                                        QMessageBox::Yes,QMessageBox::No) == QMessageBox::No) {
           return;
        }
        result = "Renuntat";
    }

    QUrlQuery query;

    query.addQueryItem("user_hash", The::user_hash());
    query.addQueryItem("contract_id",QString("%1").arg(contract_id));
    query.addQueryItem("motiv",result);

    QUrl url =  QUrl(QString(The::server_url() + "contracte/finalizare.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    emit loadingStart();
}

void MainMdi::parseAnswer(QByteArray data) {
    emit loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","MainMdi","parseAnswer();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"MainMdi","parseAnswer();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        this->getContent();
    }
}

void MainMdi::expandAll() {
    QTreeWidgetItemIterator it(ui->treeWidget);

    while (*it) {
        (*it)->setExpanded(true);
        ++it;
    }
}

void MainMdi::collapseAll() {
    QTreeWidgetItemIterator it(ui->treeWidget);

    while (*it) {
        (*it)->setExpanded(false);
        ++it;
    }
}

void MainMdi::resetPrivileges() {
    int nivel = The::nivel_user();

    this->finalizare->setEnabled(false);

    if (nivel > 0) {
        this->finalizare->setEnabled(true);
    }

    if (nivel > 1) {}

    if (nivel > 2) {
        this->finalizare->setEnabled(false);
    }

    if (nivel > 3) {
        this->finalizare->setEnabled(true);
    }
}
