#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtWidgets>
#include <QStringList>
#include <QNetworkProxy>
#include <QNetworkAccessManager>
#include <QNetworkProxyFactory>
#include <QSslSocket>
#include <cstdlib>
#include "network/networkaccess.h"
#include <QSettings>
//#include "network/json.h"

#include "parser/parser.h"
#include "serializer/serializer.h"

#include <QMovie>
#include <QDebug>

namespace The {

    extern void maybeSetSystemProxy();

    extern void networkHttpProxySetting();

    extern QNetworkAccessManager* networkAccessManager();

    extern NetworkAccess* http();

    extern QString server_url();

    extern QByteArray user_hash();

    extern void set_user_hash(QString new_hash);

    extern int nivel_user();

    extern void set_user_nivel(int new_nivel);

    extern void set_username(QString name);

    extern QString username();

    extern Parser* parser();

    extern Serializer* serializer();
}

#endif // GLOBAL_H
