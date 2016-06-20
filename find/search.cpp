#include "search.h"
#include "ui_search.h"

Search::Search(QWidget *parent) : QDialog(parent), ui(new Ui::Search) {
    ui->setupUi(this);

    ui->treeWidget->setColumnHidden(0,true);

    movie = new QMovie(":/loading");
    movie->setScaledSize(QSize(20,20));
    ui->loadingLabel->setMovie(movie);
    ui->loadingLabel->hide();

    this->menu = new QMenu(this);
    this->finalizare = new QAction(QIcon(":/finalizare"),"&Finalizare",this);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->findButton,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
    connect(ui->treeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));

    this->getFurnizori();

    QTimer::singleShot(500, ui->contractLineEdit, SLOT(setFocus()));
}

Search::~Search() {
    delete ui;
}

void Search::showContextMenu(const QPoint point) {
    if (!this->finalizare->isEnabled()) {
        return;
    }

    menu->clear();
    QTreeWidgetItem *selected = ui->treeWidget->itemAt(point);

    if (!selected) {
        return;
    }

    menu->addAction(this->finalizare);

    QAction *result = this->menu->exec(QCursor::pos());

    if (result == this->finalizare) {
        bool ok;
        int contract_id = selected->text(0).toInt(&ok);
        if (ok) {
            emit finalizareContract(contract_id);
        }
    }
}

void Search::loadingStart() {
    this->movie->start();
    ui->loadingLabel->show();
    ui->findButton->setEnabled(false);
}

void Search::loadingStop() {
    this->movie->stop();
    ui->loadingLabel->hide();
    ui->findButton->setEnabled(true);
}

void Search::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"Search","handleNetworkError();",Logger::Network);
}

void Search::getFurnizori() {
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());

    QUrl url =  QUrl(QString(The::server_url() + "search/get_detalii.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseFurnizori(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Search::parseFurnizori(QByteArray data) {
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
        Log::logger()->logError(error,"Search","parseFurnizori();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        foreach (const QVariant &furnizor, result["furnizori"].toList()) {
            ui->furnizorComboBox->addItem(furnizor.toMap()["denumire"].toString());
        }
    }
}

void Search::search() {
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());

    bool only = false;
    if (ui->contractCheckBox->isChecked()) {
        query.addQueryItem("contract",ui->contractLineEdit->text().trimmed());
        only = true;
    }

    if (ui->codCheckBox->isChecked()) {
        query.addQueryItem("cod",ui->codLineEdit->text().trimmed());
        only = false;
    }

    if (ui->denumireCheckBox->isChecked()) {
        query.addQueryItem("denumire",ui->denumireLineEdit->text().trimmed());
        only = false;
    }

    if (ui->furnizorCheckBox->isChecked()) {
        query.addQueryItem("furnizor",ui->furnizorComboBox->currentText());
        only = false;
    }

    if (only) {
        query.addQueryItem("has_contract","1");
    }

    QUrl url =  QUrl(QString(The::server_url() + "search/get_results.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parseAnswer(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void Search::parseAnswer(QByteArray data) {
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
        Log::logger()->logError(error,"Search","parseFurnizori();",Logger::Server);
        if (status == 401) {
            emit kill();
        }
        return;
    } else {
        QTreeWidgetItem *item;
        ui->treeWidget->clear();
        ui->treeWidget->setUpdatesEnabled(false);

        if (result["contract"].toBool()) {
            foreach (const QVariant &contract, result["contracte"].toList()) {
                item = new QTreeWidgetItem();
                item->setText(0,contract.toMap()["id"].toString());
                item->setText(1,contract.toMap()["contract"].toString());
                item->setText(2,contract.toMap()["auto"].toString());
                ui->treeWidget->addTopLevelItem(item);
            }
        } else {
            foreach (const QVariant &contract, result["contracte"].toList()) {
                item = new QTreeWidgetItem();
                item->setText(0,contract.toMap()["contracte"].toMap()["id"].toString());
                item->setText(1,contract.toMap()["contracte"].toMap()["contract"].toString());
                item->setText(2,contract.toMap()["contracte"].toMap()["auto"].toString());
                item->setText(3,contract.toMap()["linii"].toMap()["cod"].toString());
                item->setText(4,contract.toMap()["linii"].toMap()["denumire"].toString());
                item->setText(5,contract.toMap()["linii"].toMap()["cantitate"].toString());
                item->setText(6,contract.toMap()["linii"].toMap()["furnizor"].toString());
                ui->treeWidget->addTopLevelItem(item);
            }
            ui->treeWidget->resizeColumnToContents(4);
        }
        ui->treeWidget->setUpdatesEnabled(true);
    }
}

void Search::itemDoubleClicked(QTreeWidgetItem *item, int col) {
    Q_UNUSED(col);
    bool ok;
    int contract_id = item->text(0).toInt(&ok);
    if (ok && contract_id) {
        emit editContract(contract_id);
    }
}

void Search::resetPrivileges() {
    int nivel = The::nivel_user();

    this->finalizare->setEnabled(false);

    if (nivel > 0) {
        this->finalizare->setEnabled(true);
    }

    if (nivel > 2) {
        this->finalizare->setEnabled(false);
    }

    if (nivel > 3) {
        this->finalizare->setEnabled(true);
    }
}
