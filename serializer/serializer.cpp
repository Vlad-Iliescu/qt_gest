#include "serializer.h"

Serializer::Serializer()
{

}

QByteArray Serializer::serialize(QVariantMap &linii)
{
    QJsonObject obj = QJsonObject::fromVariantMap(linii);
    QJsonDocument *doc = new QJsonDocument(obj);
    return doc->toJson();
}
