#include "logger.h"

#include "errorqueue.h"
#include "stdfunc.h"

#include <iostream>
#define LOG_MAX_SIZE 1048576
#define LOGFILE "coma.log"

void checkNGzip(QString &fileName)
{
    QFile logFile(fileName);
    QString GZippedLogFile = fileName;
    if (logFile.size() < LOG_MAX_SIZE)
        return;
    if (!StdFunc::checkArchiveExist(GZippedLogFile))
        return;
    GZippedLogFile += ".0.gz";

    QFile fileOut;
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Cannot open the file" << fileName;
        return;
    }
    fileOut.setFileName(fileName);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "Cannot open the file" << GZippedLogFile;
        return;
    }

    fileOut.write(StdFunc::compress(logFile.readAll()));
    fileOut.close();
    logFile.close();
}
void Logging::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const char space = ' ';
    const char colon = ':';
    QStringList buffer = QString(context.file).split("\\");
    QString sourceFile = !buffer.isEmpty() ? buffer.takeLast() : "";
    QString fileName(LOGFILE);
    QFile logFile;
    QTextStream out;

    std::string function = context.function ? context.function : "";
    std::string rubbish(" __cdecl");
    StdFunc::removeSubstr(function, rubbish);

    switch (type)
    {
    case QtInfoMsg:
    {
        Error::ErMsg tmpm {
            QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"), // DateTime
            type,                                                         // Msg type
            sourceFile,                                                   // File
            context.line,                                                 // Line
            msg                                                           // Message
        };
        ErrorQueue::GetInstance().pushError(tmpm);
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    }
    case QtDebugMsg:
#ifdef QT_DEBUG
        std::cout << sourceFile.toStdString() << colon << context.line << space << function << space << colon << space
                  << msg.toStdString() << std::endl;
#endif
        break;
    case QtWarningMsg:
#ifdef QT_DEBUG
        std::cout << sourceFile.toStdString() << colon << context.line << space << colon << space << msg.toStdString()
                  << std::endl;
#endif
        break;
    case QtCriticalMsg:
    {
        Error::ErMsg tmpm {
            QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"), // DateTime
            type,                                                         // Msg type
            sourceFile,                                                   // File
            context.line,                                                 // Line
            msg                                                           // Message
        };
        ErrorQueue::GetInstance().pushError(tmpm);
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    }
    case QtFatalMsg:
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    }
    logFile.setFileName(fileName);
    out.setDevice(&logFile);
    logFile.open(QFile::Append | QFile::Text);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") // Log datetime
        << context.category << space                                         // Msg category
        << msg                                                               // Message
        << Qt::endl;

    out.flush(); // Flush buffer
    logFile.close();
    checkNGzip(fileName);
}
/// Категории мы сейчас не используем, задел на будущее
Q_LOGGING_CATEGORY(logDebug, "Debug")
Q_LOGGING_CATEGORY(logInfo, "Info")
Q_LOGGING_CATEGORY(logWarning, "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")
