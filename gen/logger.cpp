#include "logger.h"

#include "stdfunc.h"

void Logging::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString sourceFile = context.file;
    QStringList name = sourceFile.split("\\");
    name.removeLast();
    QString fileName;
    QStringList folderList { "alarm", "check", "config", "dialogs", "gen", "iec104", "modbus", "models", "usb",
        "widgets" };
    QFile logFile;
    if (name.isEmpty() || !folderList.contains(name.last()))
        fileName = ("log.txt");
    else
        fileName = (name.last() + ".txt");

    QTextStream out;

    // Detect type of msg
    switch (type)
    {
    case QtInfoMsg:
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    case QtDebugMsg:
        break;
    case QtWarningMsg:
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    case QtCriticalMsg:
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    case QtFatalMsg:
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    }
    logFile.setFileName(fileName);
    out.setDevice(&logFile);
    logFile.open(QFile::Append | QFile::Text);
    // Log datetime
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    // Write category and msg
    out << context.category << " " << sourceFile << ": " << msg << Qt::endl;

    out.flush(); // Flush buffer
}

Logging *Logging::GetInstance()
{
    if (m_instance == nullptr)
    {
        QMutexLocker locker(&m_mutex);
        if (m_instance == nullptr)
        {
            m_instance = new Board(obj);
        }
    }
    return m_instance;
}

Q_LOGGING_CATEGORY(logDebug, "Debug")
Q_LOGGING_CATEGORY(logInfo, "Info")
Q_LOGGING_CATEGORY(logWarning, "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")
