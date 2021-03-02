#include "logclass.h"

#include "stdfunc.h"

#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMutexLocker>

LogClass::LogClass(QObject *parent) : QObject(parent)
{
    fp = nullptr;
    mtx = new QMutex;
    CanLog = false;
}

LogClass::~LogClass()
{
    if (fp != nullptr)
    {
        fp->flush();
        fp->close();
        delete fp;
    }
    delete mtx;
}

void LogClass::Init(const QString &Filename)
{
    LogFile = StdFunc::GetSystemHomeDir() + Filename;
    // тестовая проверка открытия файла на запись
    fp = new QFile(LogFile);
    if (!fp->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        CanLog = false;
        qCritical("Ошибка открытия файла");
        return;
    }
    CanLog = true;
}

void LogClass::error(const QString &str)
{
    if (CanLog)
        WriteFile("Error", str);
}

void LogClass::info(const QString &str)
{
    if (CanLog)
        WriteFile("Info", str);
}

void LogClass::warning(const QString &str)
{
    if (CanLog)
        WriteFile("Warning", str);
}

void LogClass::intvarvalue(const QString &var, int value)
{
    if (CanLog)
        WriteFile(var, QString::number(value));
}

void LogClass::WriteFile(const QString &Prepend, const QString &msg)
{
    if (fp != nullptr)
    {
        QMutexLocker locker(mtx);
        QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "]";
        fp->write(tmps.toLocal8Bit());
        tmps = "[" + Prepend + "] ";
        fp->write(tmps.toLocal8Bit());
        fp->write(msg.toLocal8Bit());
        fp->write("\n");
        fp->flush();
        CheckAndCompress();
    }
}

// thread-safe function

void LogClass::WriteRaw(const QByteArray &ba)
{
    if (fp != nullptr)
    {
        QMutexLocker locker(mtx);
        QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "]";
        int writtenSize;
        writtenSize = fp->write(tmps.toLocal8Bit());
        if (writtenSize == -1)
            return;
        writtenSize = fp->write(ba);
        if (writtenSize == -1)
            return;
        if (!fp->flush())
            return;
        CheckAndCompress();
    }
}

void LogClass::CheckAndCompress()
{
    QString GZippedLogFile = LogFile;
    if (fp->size() < LOG_MAX_SIZE)
        return;
    if (!StdFunc::checkArchiveExist(GZippedLogFile))
        return;
    GZippedLogFile += ".0.gz";

    QFile fileIn, fileOut;
    fileIn.setFileName(LogFile);
    if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open the file" << LogFile;
        return;
    }
    fileOut.setFileName(GZippedLogFile);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qCritical() << "Cannot open the file" << GZippedLogFile;
        return;
    }
    fp->close();

    fileOut.write(StdFunc::compress(fileIn.readAll()));
    fileIn.close();
    fileOut.close();
    fp->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
}
