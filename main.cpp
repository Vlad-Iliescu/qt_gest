#include <QtWidgets/QApplication>
#include "qtgest.h"
#include "login/login.h"

#include "log.h"

bool checkForUpdates(QString path) {
    QProcess updater;
    QStringList args = QStringList() << "-quickcheck" << "-justcheck";

    int exit = updater.execute(path+"/wyUpdate", args);

    return exit == 2;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap pixmap(":/splash");

    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();


    // check for updates
    QString path = a.applicationDirPath();
    if (0 && checkForUpdates(path)) {
        QProcess updater;
        updater.startDetached("wyUpdate");

        if (updater.waitForStarted()) {

        }
        a.quit();
    }
    // end check for updates

    QtGest *mw = 0;
    Login *ld = new Login();

    splash->finish(ld);

    int status;

    do {
        if (ld->exec()) {
            if (!mw) {
                mw = new QtGest();
            }

            mw->resetPrivileges();
            mw->setWindowTitle(QString("QtGest - %1").arg(ld->getUsername()));
            mw->setUsername(ld->getUsername());
            mw->showMaximized();
            a.exec();
            status = 1;
        } else {
            status = 0;
        }
    } while (status != 0);

    return status;
}
