#include "networkaccess.h"
#include "global.h"

#include <QtWidgets>

#define USER_AGENT  "QtGest RoKoR"
QT_BEGIN_NAMESPACE

namespace The {
    NetworkAccess* http();
}

NetworkReply::NetworkReply(QNetworkReply *networkReply) : QObject(networkReply) {
    this->networkReply = networkReply;
    networkReply->ignoreSslErrors();
}

QNetworkReply *NetworkReply::getNetworkReply()
{
//    qDebug() << "Aborted";
    return networkReply;
}

void NetworkReply::abortReply() {
    this->getNetworkReply()->abort();
}

void NetworkReply::finished() {

//     qDebug() << "NETWORK ACCESS -> FINISHED() : " << networkReply->url().toString();
    QUrl redirection = networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (redirection.isValid()) {

//        qDebug() << "REDIRECT! : "  << redirection;

       QNetworkReply *redirectReply = The::http()->simpleGet(redirection, networkReply->operation());

        setParent(redirectReply);
        networkReply->deleteLater();
        networkReply = redirectReply;

        // when the request is finished we'll invoke the target method
        connect(networkReply, SIGNAL(finished()), this, SLOT(finished()), Qt::AutoConnection);
        connect(networkReply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
        return;
    }


    emit finished(networkReply);

    // get the HTTP response body
    QByteArray bytes = networkReply->readAll();

    emit data(bytes);

    // bye bye my reply
    // this will also delete this NetworkReply as the QNetworkReply is its parent
    networkReply->deleteLater();
}

void NetworkReply::requestError(QNetworkReply::NetworkError code)
{
     qDebug() << "NETWORK ACCESS -> requestError : " << networkReply->errorString() << code;
    emit error(networkReply);
}

void NetworkReply::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
//     qDebug() << "NETWORK ACCESS -> downloadProgress : " << bytesReceived << " / " << bytesTotal;
    emit progress(bytesReceived,bytesTotal);
}


/* --- NetworkAccess --- */

NetworkAccess::NetworkAccess( QObject* parent) : QObject( parent ) {}

QNetworkReply* NetworkAccess::simpleGet(QUrl url, int operation) {

//    qDebug() << "NETWORK ACCESS -> SIMPLE GET : " << url.toString() << operation;
    QNetworkAccessManager *manager = The::networkAccessManager();


    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", QString(USER_AGENT).toUtf8());
    request.setRawHeader("Connection", "Keep-Alive");

    QNetworkReply *networkReply;
    switch (operation) {

    case QNetworkAccessManager::GetOperation:
//        qDebug() << "GET" << url.toString();
        networkReply = manager->get(request);
        break;

    case QNetworkAccessManager::HeadOperation:
//        qDebug() << "HEAD" << url.toString();
        networkReply = manager->head(request);
        break;

    default:
//        qDebug() << "Unknown operation:" << operation;
        return 0;

    }

    // error handling
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(error(QNetworkReply::NetworkError)));

    return networkReply;

}

QNetworkReply* NetworkAccess::simplePost(QUrl url, QByteArray data, int operation) {

//    qDebug() << "NETWORK ACCESS -> SIMPLE GET : " << url.toString() << operation;
    QNetworkAccessManager *manager = The::networkAccessManager();

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", QString(USER_AGENT).toUtf8());
    request.setRawHeader("Connection", "Keep-Alive");

    QNetworkReply *networkReply;

    switch (operation) {
        case QNetworkAccessManager::HeadOperation:
//        qDebug() << "HEAD" << url.toString();
            networkReply = manager->head(request);
            break;
        case QNetworkAccessManager::GetOperation:
//        qDebug() << "GET" << url.toString();
            networkReply = manager->get(request);
            break;
        case QNetworkAccessManager::PostOperation:
//        qDebug() << "POST" << url.toString();
            networkReply = manager->post(request,data);
            break;
        default:
            return 0;
    }

    // error handling
    connect(networkReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));

    return networkReply;
}

NetworkReply* NetworkAccess::post(QUrl url, QByteArray query) {
//    qDebug() << "NETWORK ACCESS -> POST() : " << url.toString();

    QNetworkReply *networkReply = simplePost(url,query);
    NetworkReply *reply = new NetworkReply(networkReply);

    connect(networkReply,SIGNAL(error(QNetworkReply::NetworkError)),reply,SLOT(requestError(QNetworkReply::NetworkError)));
    connect(networkReply, SIGNAL(downloadProgress(qint64,qint64)),reply,SLOT(downloadProgress(qint64,qint64)));

    connect(networkReply,SIGNAL(finished()),reply,SLOT(finished()),Qt::AutoConnection);

    return reply;
}

NetworkReply* NetworkAccess::get(const QUrl url) {

//    qDebug() << "NETWORK ACCESS -> GET() : " << url.toString();

    QNetworkReply *networkReply = simpleGet(url);
    NetworkReply *reply = new NetworkReply(networkReply);

    // error signal
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),reply, SLOT(requestError(QNetworkReply::NetworkError)));
    connect(networkReply,SIGNAL(downloadProgress(qint64,qint64)),reply,SLOT(downloadProgress(qint64,qint64)));

    // when the request is finished we'll invoke the target method
    connect(networkReply, SIGNAL(finished()), reply, SLOT(finished()), Qt::AutoConnection);

    return reply;

}

NetworkReply* NetworkAccess::head(const QUrl url) {

//    qDebug() << "NETWORK ACCESS -> HEAD() : " << url.toString();

    QNetworkReply *networkReply = simpleGet(url, QNetworkAccessManager::HeadOperation);
    NetworkReply *reply = new NetworkReply(networkReply);

    // error signal
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),reply, SLOT(requestError(QNetworkReply::NetworkError)));

    // when the request is finished we'll invoke the target method
    connect(networkReply, SIGNAL(finished()), reply, SLOT(finished()), Qt::AutoConnection);

    return reply;

}

/*** sync ***/


QNetworkReply* NetworkAccess::syncGet(QUrl url) {

    working = true;

    networkReply = simpleGet(url);

    connect(networkReply, SIGNAL(metaDataChanged()),this, SLOT(syncMetaDataChanged()), Qt::AutoConnection);
    connect(networkReply, SIGNAL(finished()),this, SLOT(syncFinished()), Qt::AutoConnection);
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(error(QNetworkReply::NetworkError)));

    // A little trick to make this function blocking
    while (working) {
        // Do something else, maybe even network processing events
        qApp->processEvents();
        qApp->changeOverrideCursor(Qt::WaitCursor);
    }
        qApp->restoreOverrideCursor();
    networkReply->deleteLater();
    return networkReply;

}

QNetworkReply* NetworkAccess::syncPost(QUrl url, QByteArray query) {

    working = true;

    networkReply = simplePost(url,query);

    connect(networkReply, SIGNAL(metaDataChanged()),this,SLOT(syncMetaDataChanged()),Qt::AutoConnection);
    connect(networkReply, SIGNAL(finished()),this,SLOT(syncFinished()),Qt::AutoConnection);
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));

    while (working) {
        // Do something else, maybe even network processing events
        qApp->processEvents();
        qApp->changeOverrideCursor(Qt::WaitCursor);
    }
        qApp->restoreOverrideCursor();

//    QByteArray bytes = networkReply->readAll();

    networkReply->deleteLater();

    return networkReply;
}

void NetworkAccess::syncMetaDataChanged() {

    QUrl redirection = networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (redirection.isValid()) {

//        qDebug() << "Redirect" << redirection;
        networkReply->deleteLater();
        syncGet(redirection);


        QNetworkAccessManager *manager = The::networkAccessManager();
        networkReply->deleteLater();
        networkReply = manager->get(QNetworkRequest(redirection));
        connect(networkReply, SIGNAL(metaDataChanged()),this, SLOT(metaDataChanged()), Qt::AutoConnection);
        connect(networkReply, SIGNAL(finished()),this, SLOT(finished()), Qt::AutoConnection);
    }

}

void NetworkAccess::syncFinished() {
    // got it!
    working = false;
}

void NetworkAccess::error(QNetworkReply::NetworkError code) {
    Q_UNUSED(code);
    // get the QNetworkReply that sent the signal
    QNetworkReply *networkReply = static_cast<QNetworkReply *>(sender());
    if (!networkReply) {
//        qDebug() << "Cannot get sender";
        return;
    }

    // Ignore HEADs
    if (networkReply->operation() == QNetworkAccessManager::HeadOperation)
        return;

    // report the error in the status bar
    QMainWindow* mainWindow = dynamic_cast<QMainWindow*>(qApp->topLevelWidgets().first());
    if (mainWindow) mainWindow->statusBar()->showMessage(
            tr("Network error: %1").arg(networkReply->errorString()));

//    qDebug() << "Network error:" << networkReply->errorString() << code;

    networkReply->deleteLater();
}

QByteArray NetworkAccess::syncGetBytes(QUrl url) {
    return syncGet(url)->readAll();
}

QString NetworkAccess::syncGetString(QUrl url) {
    return QString::fromUtf8(syncGetBytes(url));
}

QByteArray NetworkAccess::syncPostBytes(QUrl url, QByteArray query) {
    return syncPost(url,query)->readAll();
}

QT_END_NAMESPACE
