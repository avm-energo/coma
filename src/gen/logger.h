#pragma once

#include <QLoggingCategory>
#include <QtCore>

Q_DECLARE_LOGGING_CATEGORY(logDebug)
Q_DECLARE_LOGGING_CATEGORY(logInfo)
Q_DECLARE_LOGGING_CATEGORY(logWarning)
Q_DECLARE_LOGGING_CATEGORY(logCritical)

class Logging
{
public:
    void static messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void static writeStart();

protected:
    Logging() = delete;
};
