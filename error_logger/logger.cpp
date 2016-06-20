#include "logger.h"

Logger::Logger(QObject *parent) : QObject(parent) {
    file = new QFile("Errors.log");
    out = new QTextStream(file);
    out->setCodec("UTF-8");
}

void Logger::logError(QString error, QString clas, QString function, ErrorType type) {
    QString errorType;
    if (type == Logger::Network) {
        errorType = "Network Error";
    } else if (type == Logger::JSON) {
        errorType = "JSON Error";
    } else if (type == Logger::User) {
        errorType = "User Error";
    } else if (type == Logger::Session) {
        errorType = "Session Expired";
    } else if (type == Logger::Server) {
        errorType = "Server Error";
    } else {
        errorType = "Other";
    }

    if (file->open(QIODevice::Append | QIODevice::Text)) {
        QString timestamp = QDateTime::currentDateTime().toString("[dd-MM-yyyy hh:mm:ss]");
        *out << timestamp;
        *out << "[" + errorType + "]";
        *out << "[" + clas + "]";
        *out << "[" + function + "]";
        *out << "[" + error + "]";
        *out << "\n";

        file->close();
    }
}
