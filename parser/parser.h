#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QJsonDocument>
#include <QVariant>

class Parser
{
public:
    Parser();
    QVariant parse(const QByteArray& jsonData, bool* ok = 0);
};

#endif // PARSER_H
