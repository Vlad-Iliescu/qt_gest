#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QtNetwork>

namespace The {
    QNetworkAccessManager* networkAccessManager();
}

class NetworkReply : public QObject {

    Q_OBJECT

public:
    NetworkReply(QNetworkReply* networkReply);

public slots:
    void finished();
    void requestError(QNetworkReply::NetworkError);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    QNetworkReply *getNetworkReply();
    void abortReply();

signals:
    void progress(qint64, qint64);
    void data(QByteArray);
    void error(QNetworkReply*);
    void finished(QNetworkReply*);

private:
    QNetworkReply *networkReply;

};


class NetworkAccess : public QObject {

    Q_OBJECT

public:
    NetworkAccess( QObject* parent=0);
    QNetworkReply* simpleGet(QUrl url, int operation = QNetworkAccessManager::GetOperation);
    QNetworkReply* simplePost(QUrl url, QByteArray data, int operation = QNetworkAccessManager::PostOperation);

    NetworkReply* head(QUrl url);

    NetworkReply* get(QUrl url);
    QByteArray syncGetBytes(QUrl url);
    QNetworkReply* syncGet(QUrl url);
    QString syncGetString(QUrl url);

    NetworkReply* post(QUrl url, QByteArray query);
    QNetworkReply* syncPost(QUrl url, QByteArray query);
    QByteArray syncPostBytes(QUrl url, QByteArray query);

private slots:
    void error(QNetworkReply::NetworkError);
    void syncMetaDataChanged();

    void syncFinished();

private:
    QNetworkReply *networkReply;
    bool working;

};

typedef QPointer<QObject> ObjectPointer;
Q_DECLARE_METATYPE(ObjectPointer)

#endif // NETWORKACCESS_H
