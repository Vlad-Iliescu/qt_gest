#include "finalizate.h"
#include "ui_finalizate.h"

Finalizate::Finalizate(QWidget *parent) : QWidget(parent), ui(new Ui::Finalizate) {
    ui->setupUi(this);

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
    ui->treeWidget->header()->setSectionResizeMode(10,QHeaderView::ResizeToContents);

    this->setAttribute(Qt::WA_DeleteOnClose);

    QTimer *refresh = new QTimer(this);
    connect(refresh,SIGNAL(timeout()),this,SLOT(getContent()));
    refresh->start(300000);

    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(editeazaContract(QTreeWidgetItem*,int)));
}

Finalizate::~Finalizate() {
    delete ui;
}

void Finalizate::getContent() {
    QUrlQuery query;

    query.addQueryItem("user_hash", The::user_hash());

    QUrl url =  QUrl(QString(The::server_url() + "contracte/get_contracte_finalizate.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseContent(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    emit loadingStart();
}

void Finalizate::parseContent(QByteArray data) {
    emit loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","Finalizate","parseContent();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"Finalizate","parseContent();",Logger::Server);
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

            temp = contract.toMap()["id"];
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

            temp = contract.toMap()["motiv"];
            item->setText(9,temp.toString());

            temp = contract.toMap()["observatii"];
            item->setText(10,temp.toString());

            ui->treeWidget->addTopLevelItem(item);
        }
        ui->treeWidget->setUpdatesEnabled(true);
    }
}

void Finalizate::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"Finalizate","handleNetworkError();",Logger::Network);
}

void Finalizate::editeazaContract(QTreeWidgetItem *item, int col) {
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

void Finalizate::editeazaContract(int contract_id) {
    emit editContract(contract_id);
}
