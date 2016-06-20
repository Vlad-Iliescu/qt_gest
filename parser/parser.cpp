#include "parser.h"

Parser::Parser()
{

}

QVariant Parser::parse(const QByteArray &jsonData, bool *ok)
{
    QJsonParseError *error = NULL;
    QJsonDocument sd = QJsonDocument::fromJson(jsonData, error);
    *ok = error == NULL;
    return sd.toVariant();
}
