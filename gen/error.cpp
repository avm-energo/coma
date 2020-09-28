#include "error.h"

#include <QDateTime>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

QStringList Error::ErrMsgs;
QList<Error::ErMsg> Error::ErMsgPool;
LogClass Error::LogFile;

Error::Error()
{
}

void Error::Init()
{
    LogFile.Init(LOGFILE);
    LogFile.info("=== Log started ===\n");
    QFile file;
    QString ermsgspath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    file.setFileName(ermsgspath + "ermsgs.dat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ERMSG("Ошибка открытия файла");
        return;
    }
    QString tmpString;
    QTextStream streamfile(&file);
    streamfile.setCodec("WIN-1251");
    do
    {
        tmpString = streamfile.readLine();
        if (tmpString.isEmpty())
            ErrMsgs << "";
        else if (tmpString.at(0) != '#')
            ErrMsgs << tmpString;
    } while (!streamfile.atEnd());
}

void Error::AddErrMsg(ErMsgType msgtype, QString file, int line, QString msg)
{
    if (ErMsgPool.size() >= ER_BUFMAX)
        ErMsgPool.removeFirst();
    ErMsg tmpm;
    tmpm.type = msgtype;
    tmpm.file = file;
    tmpm.line = line;
    tmpm.DateTime = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");
    // Разбор кода ошибки

    if ((msgtype == ER_MSG) || (msgtype == DBG_MSG))
        LogFile.error("file: " + tmpm.file + ", line: " + QString::number(tmpm.line) + ": " + msg);
    else if (msgtype == WARN_MSG)
        LogFile.warning("file: " + tmpm.file + ", line: " + QString::number(tmpm.line) + ": " + msg);
    else
        LogFile.info("file: " + tmpm.file + ", line: " + QString::number(tmpm.line) + ": " + msg);
    tmpm.msg = msg;
    ErMsgPool.append(tmpm);
}

void Error::ShowErMsg(Error::Msg msg)
{
    ERMSG(QVariant::fromValue(msg).toString());
}

int Error::ErMsgPoolSize()
{
    return ErMsgPool.size();
}

Error::ErMsg Error::ErMsgAt(int idx)
{
    if (idx < ErMsgPool.size())
        return ErMsgPool.at(idx);
    else
        return ErMsg();
}
