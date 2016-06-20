#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>

class Serializer
{
public:
    Serializer();
    QByteArray serialize(QVariantMap &linii);
};

#endif // SERIALIZER_H
