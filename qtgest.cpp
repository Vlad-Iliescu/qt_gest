#include <QUrlQuery>
#include "qtgest.h"
#include "ui_qtgest.h"

#include "global.h"
#include "log.h"

QtGest::QtGest(QWidget *parent) : QMainWindow(parent), ui(new Ui::QtGest) {
    ui->setupUi(this);

//    this->setAttribute(Qt::WA_DeleteOnClose);
    this->username = new QLabel(this);
    this->todo = new ToDo(this);
    connect(this->todo,SIGNAL(kill()),this,SLOT(logout()));

    this->createLoadingLabel();
    this->setUpMainMdi();

    this->addContract = 0;
    this->TVA = 0.24;

    this->todoTimer = new QTimer();
    this->todoTimer->start(300000);

//    ui->action_Adaugare->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    ui->action_Logout->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    ui->action_Find->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

    connect(ui->action_Adaugare,SIGNAL(triggered()),this,SLOT(adaugareContract()));
    connect(ui->actionEditare,SIGNAL(triggered()),this->mainMdi,SLOT(editeazaContract()));
    connect(ui->action_Finalizare,SIGNAL(triggered()),this->mainMdi,SLOT(finalizeazaContract()));
    connect(ui->action_Logout,SIGNAL(triggered()),this,SLOT(logout()));
    connect(ui->actionC_ascade_Window,SIGNAL(triggered()),ui->mdiArea,SLOT(cascadeSubWindows()));
    connect(ui->action_Tile_Windows,SIGNAL(triggered()),ui->mdiArea,SLOT(tileSubWindows()));
    connect(ui->action_Collapse_All,SIGNAL(triggered()),this->mainMdi,SLOT(collapseAll()));
    connect(ui->action_Expand_All,SIGNAL(triggered()),this->mainMdi,SLOT(expandAll()));
    connect(ui->action_Furnizori,SIGNAL(triggered()),this,SLOT(furnizori()));
    connect(ui->action_Asiguratori,SIGNAL(triggered()),this,SLOT(asiguratori()));
    connect(ui->action_Users,SIGNAL(triggered()),this,SLOT(users()));
    connect(ui->actionR_apoarte,SIGNAL(triggered()),this,SLOT(rapoarte()));
    connect(ui->action_Find,SIGNAL(triggered()),this,SLOT(search()));
    connect(ui->actionF_inalizate,SIGNAL(triggered()),this,SLOT(finalizate()));

    connect(this->todoButton,SIGNAL(clicked()),this,SLOT(popupToDo()));
    connect(todo,SIGNAL(editContract(int)),this,SLOT(editareContact(int)));
    connect(this->todoTimer,SIGNAL(timeout()),this,SLOT(popupToDo()));

//    ui->mdiArea->setTabShape(QTabWidget::Triangular);
}


void QtGest::createLoadingLabel() {
    this->loadingLabel = new QLabel();
    this->movie = new QMovie(":/loading");
    this->movie->setScaledSize(QSize(20,20));
    this->loadingLabel->setMovie(this->movie);


    this->todoButton = new QPushButton(this);
    this->todoButton->setMaximumHeight(20);
    this->todoButton->setMaximumWidth(20);
    this->todoButton->setMinimumHeight(20);
    this->todoButton->setMinimumWidth(20);

    this->todoButton->setText("0");
    this->todoButton->setStyleSheet("background-color: qradialgradient(spread:pad, "
                                        "cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, "
                                        "stop:0 rgba(200, 255, 200, 255), stop:1 rgba(0, 255, 0, 255));"
                                    "border: 0 none;"
                                    "border-radius: 10px 10px 10px 10px;");

    ui->statusBar->addPermanentWidget(this->username);
    ui->statusBar->addPermanentWidget(this->loadingLabel);
    ui->statusBar->addPermanentWidget(this->todoButton);
}

void QtGest::setUpMainMdi() {
    this->mainMdi = new MainMdi();
    connect(this->mainMdi,SIGNAL(loadingStart()),this,SLOT(loadingStart()));
    connect(this->mainMdi,SIGNAL(loadingStop()),this,SLOT(loadingStop()));
    connect(this->mainMdi,SIGNAL(tva(double)),this,SLOT(setTva(double)));
    connect(this->mainMdi,SIGNAL(editContract(int)),this,SLOT(editareContact(int)));
    connect(this->mainMdi,SIGNAL(kill()),this,SLOT(logout()));

    this->mainMdi->getContent();
    this->refresh = new QTimer(this);
    this->refresh->start(60*1000);

    connect(this->refresh,SIGNAL(timeout()),this->mainMdi,SLOT(getContent()));
    connect(ui->action_Refresh,SIGNAL(triggered()),this->mainMdi,SLOT(getContent()));

    QMdiSubWindow *list = ui->mdiArea->addSubWindow(mainMdi, (Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint));   

    list->setGeometry(10,0,1024,600);

    mainMdi->show();
    this->popupToDo();
}

void QtGest::setUsername(QString username) {
    this->username->setText(username);
    this->username->setStyleSheet("color: red; font-weight: bold;");
}

QtGest::~QtGest() {
    qDebug() << "deleted";
    delete ui;
}

void QtGest::setTva(double tva) {
    this->TVA = tva;
}

void QtGest::loadingStart() {
    this->movie->start();
    this->loadingLabel->show();
}

void QtGest::loadingStop() {
    this->movie->stop();
    this->loadingLabel->hide();
}

void QtGest::adaugareContract() {
    if (!ui->action_Adaugare->isEnabled()) {
        return;
    }

    if (!this->addContract) {
        this->addContract = new AddContract(this);
        connect(this->addContract,SIGNAL(kill()),this,SLOT(logout()));
    }
    int contract_id = this->addContract->exec();

    if (contract_id) {
        this->editareContact(contract_id);
    }
}

void QtGest::editareContact(int contract_id) {
    if (!ui->actionEditare->isEnabled()) {
        return;
    }

    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    for (int i = 0; i < windows.size(); ++i) {
        Edit *child = qobject_cast<Edit *>(windows.at(i)->widget());
        if (child) {
            if (child->getContractId() == contract_id) {
                ui->mdiArea->setActiveSubWindow(windows.at(i));
                return;
            }
        }
    }

    Edit *e = new Edit(this);
    e->getDetaliiContract(contract_id);

    QMdiSubWindow *edit = new QMdiSubWindow();
    edit->setAttribute(Qt::WA_DeleteOnClose);
    edit->setWidget(e);

    connect(e,SIGNAL(rejected()),edit,SLOT(close()));
    connect(e,SIGNAL(kill()),this,SLOT(logout()));
    ui->mdiArea->addSubWindow(edit,(edit->windowFlags() | Qt::CustomizeWindowHint));

    int childCount = ui->mdiArea->subWindowList().count();

    edit->setGeometry(10*childCount, 30*childCount,1024,600);

    edit->show();
    ui->mdiArea->setActiveSubWindow(edit);
    edit->setFocus();
}

void QtGest::popupToDo() {
    QUrl query_url;
    QUrlQuery query;

    query.addQueryItem("user_hash",The::user_hash());
//    query.query(QUrl::FullyEncoded).toUtf8();

    QUrl url = QUrl(QString(The::server_url() + "todo/popup.json"));
    QObject *post = The::http()->post(url, query.query(QUrl::FullyEncoded).toUtf8());

    connect(post, SIGNAL(data(QByteArray)),this, SLOT(parsePopup(QByteArray)));
    connect(post, SIGNAL(error(QNetworkReply*)), this, SLOT(handleNetworkError(QNetworkReply*)));
    loadingStart();
}

void QtGest::parsePopup(QByteArray data) {
    loadingStop();

    bool ok;
    QVariantMap result = The::parser()->parse(data,&ok).toMap();

    if (!ok) {
        QMessageBox::warning(this,"Eroare Parsare","Eroare parsare JSON!");
        Log::logger()->logError("Eroare parsare JSON","QtGest","parsePopup();",Logger::JSON);
        return;
    }

    int status = result["status"].toInt(&ok);
    QString error = result["error"].toString();

    if (!ok || (status != 200)) {
        QMessageBox::warning(this,"Eroare Server",error);
        Log::logger()->logError(error,"QtGest","parsePopup();",Logger::Server);
        if (status == 401) {
            this->logout();
        }
        return;
    } else {
        int count = result["count"].toInt(&ok);
        if (!ok) {
            return;
        }
        if (!count) {
            this->todoButton->setText("0");
            this->todoButton->setStyleSheet("background-color: qradialgradient(spread:pad, "
                                                "cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, "
                                                "stop:0 rgba(200, 255, 200, 255), stop:1 rgba(0, 255, 0, 255));"
                                            "border: 0 none;"
                                            "border-radius: 10px 10px 10px 10px;");

        } else {
            this->todoButton->setText(QString("%1").arg(count));
            this->todoButton->setStyleSheet("background-color: qradialgradient(spread:pad, "
                                                "cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, "
                                                "stop:0 rgba(255, 220, 220, 255), stop:1 rgba(255, 0, 0, 255));"
                                            "border: 0 none;"
                                            "border-radius: 10px 10px 10px 10px;");
            this->todo->parsePopup(result);
        }
    }
}

void QtGest::handleNetworkError(QNetworkReply *reply) {
    Log::logger()->logError(reply->errorString(),"QtGest","handleNetworkError();",Logger::Network);
}

void QtGest::logout() {
    QList<QMdiSubWindow *> windowList =  ui->mdiArea->subWindowList();

    foreach(QMdiSubWindow *win,windowList) {
        win->close();
    }

    this->close();
}

void QtGest::furnizori() {
    if (!ui->action_Furnizori->isEnabled()) {
        return;
    }

    Furnizori *f = new Furnizori(this);
    f->getFurnizori();

    QMdiSubWindow *furn = new QMdiSubWindow();
    furn->setAttribute(Qt::WA_DeleteOnClose);
    furn->setWidget(f);

    connect(f,SIGNAL(rejected()),furn,SLOT(close()));
    connect(f,SIGNAL(kill()),this,SLOT(logout()));
    ui->mdiArea->addSubWindow(furn,(furn->windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));

    ui->mdiArea->setActiveSubWindow(furn);
    furn->show();
}

void QtGest::asiguratori() {
    if (!ui->action_Asiguratori->isEnabled()) {
        return;
    }

    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    for (int i = 0; i < windows.size(); ++i) {
        Asiguratori *child = qobject_cast<Asiguratori *>(windows.at(i)->widget());
        if (child) {
            ui->mdiArea->setActiveSubWindow(windows.at(i));
            return;
        }
    }

    Asiguratori *a = new Asiguratori(this);
    a->getAsiguratori();

    QMdiSubWindow *asig = new QMdiSubWindow();
    asig->setAttribute(Qt::WA_DeleteOnClose);
    asig->setWidget(a);

    connect(a,SIGNAL(rejected()),asig,SLOT(close()));
    connect(a,SIGNAL(kill()),this,SLOT(logout()));
    ui->mdiArea->addSubWindow(asig,(asig->windowFlags()));

    ui->mdiArea->setActiveSubWindow(asig);
    asig->show();
}

void QtGest::users() {
    if (!ui->action_Users->isEnabled()) {
        return;
    }

    Users *u = new Users(this);
    u->getUsers();

    QMdiSubWindow *usr = new QMdiSubWindow();
    usr->setAttribute(Qt::WA_DeleteOnClose);
    usr->setWidget(u);

    connect(u,SIGNAL(rejected()),usr,SLOT(close()));
    connect(u,SIGNAL(kill()),this,SLOT(logout()));
    ui->mdiArea->addSubWindow(usr,(usr->windowFlags()));

    usr->show();
    ui->mdiArea->setActiveSubWindow(usr);
}

void QtGest::resetPrivileges() {
    int nivel = The::nivel_user();

    ui->action_Adaugare->setEnabled(false);
    ui->actionEditare->setEnabled(false);
    ui->action_Finalizare->setEnabled(false);
    ui->menu_Admin->setEnabled(false);

    if (nivel > 0) {
        ui->action_Adaugare->setEnabled(true);
        ui->actionEditare->setEnabled(true);
    }

    if (nivel > 1) {}

    if (nivel > 2) {
        ui->action_Adaugare->setEnabled(false);
    }

    if (nivel > 3) {
        ui->action_Adaugare->setEnabled(true);
        ui->action_Finalizare->setEnabled(true);
        ui->menu_Admin->setEnabled(true);
    }

    this->mainMdi->resetPrivileges();
}

void QtGest::rapoarte() {
    if (!ui->actionR_apoarte->isEnabled()) {
        return;
    }

    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    for (int i = 0; i < windows.size(); ++i) {
        Rapoarte *child = qobject_cast<Rapoarte *>(windows.at(i)->widget());
        if (child) {
            ui->mdiArea->setActiveSubWindow(windows.at(i));
            return;
        }
    }


    Rapoarte *r = new Rapoarte(this);
    r->resetPrivileges();
    r->getRapoarte();
    connect(r,SIGNAL(raportReady(QVariantMap)),this,SLOT(raport(QVariantMap)));

    QMdiSubWindow *rap = new QMdiSubWindow();
    rap->setAttribute(Qt::WA_DeleteOnClose);
    rap->setWidget(r);

    connect(r,SIGNAL(rejected()),rap,SLOT(close()));
    ui->mdiArea->addSubWindow(rap,(rap->windowFlags()));

    int childCount = ui->mdiArea->subWindowList().count();

    rap->setGeometry(10*childCount, 30*childCount,1024,600);

    rap->show();
    ui->mdiArea->setActiveSubWindow(rap);
}

void QtGest::raport(QVariantMap data) {

    Raport *r = new Raport(this);
    r->populate(data);

    QMdiSubWindow *edit = new QMdiSubWindow();
    edit->setAttribute(Qt::WA_DeleteOnClose);
    edit->setWidget(r);

    connect(r,SIGNAL(rejected()),edit,SLOT(close()));
    ui->mdiArea->addSubWindow(edit,(edit->windowFlags()));

    int childCount = ui->mdiArea->subWindowList().count();

    edit->setGeometry(10*childCount, 30*childCount,1024,600);

    edit->show();
    ui->mdiArea->setActiveSubWindow(edit);
}

void QtGest::search() {
    Search *s = new Search(this);

    QMdiSubWindow *edit = new QMdiSubWindow();
    edit->setAttribute(Qt::WA_DeleteOnClose);
    edit->setWidget(s);

    connect(s,SIGNAL(rejected()),edit,SLOT(close()));
    connect(s,SIGNAL(editContract(int)),this,SLOT(editareContact(int)));
    connect(s,SIGNAL(finalizareContract(int)),this->mainMdi,SLOT(finalizeazaContract(int)));
    connect(s,SIGNAL(kill()),this,SLOT(logout()));

    ui->mdiArea->addSubWindow(edit, (edit->windowFlags()));


    int childCount = ui->mdiArea->subWindowList().count();

    edit->setGeometry(10*childCount, 30*childCount,1024,600);

    edit->show();
    ui->mdiArea->setActiveSubWindow(edit);
}

void QtGest::finalizate() {
    if (!ui->actionEditare->isEnabled()) {
        return;
    }

    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    for (int i = 0; i < windows.size(); ++i) {
        Finalizate *child = qobject_cast<Finalizate *>(windows.at(i)->widget());
        if (child) {
            ui->mdiArea->setActiveSubWindow(windows.at(i));
            return;
        }
    }

    Finalizate *e = new Finalizate(this);

    QMdiSubWindow *edit = new QMdiSubWindow();
    edit->setAttribute(Qt::WA_DeleteOnClose);
    edit->setWidget(e);
    e->getContent();

    connect(e,SIGNAL(kill()),this,SLOT(logout()));
    connect(e,SIGNAL(editContract(int)),this,SLOT(editareContact(int)));
    ui->mdiArea->addSubWindow(edit,(edit->windowFlags()));

    int childCount = ui->mdiArea->subWindowList().count();

    edit->setGeometry(10*childCount, 30*childCount,1024,600);

    edit->show();
    ui->mdiArea->setActiveSubWindow(edit);
    edit->setFocus();
}
