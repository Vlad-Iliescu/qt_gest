#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = 0);
    enum ErrorType {
        Network = 0x1,
        User    = 0x2,
        JSON    = 0x3,
        Session = 0x4,
        Server  = 0x5,
        Other   = 0x6
    };

signals:

private:
    QDateTime date;
    QFile *file;
    QTextStream *out;

public slots:
    void logError(QString error, QString clas = "", QString function = "", ErrorType type = Other);
};

#endif // LOGGER_H
