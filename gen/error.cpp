#include <QDateTime>
#include <QStandardPaths>
#include <QTextStream>
#include <QFile>
#include "error.h"

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
    file.setFileName(ermsgspath+"ermsgs.dat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
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
    if (ErMsgPool.size()>=ER_BUFMAX)
        ErMsgPool.removeFirst();
    ErMsg tmpm;
    tmpm.type = msgtype;
    tmpm.file = file;
    tmpm.line = line;
    tmpm.DateTime = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");
    // Разбор кода ошибки
    QString prefix;
    if ((msg.isEmpty()) || (msg == " ")) // пробел выдаётся при пустом запросе в БД
    {
        switch (msgtype)
        {
        case ER_MSG:
            prefix = "Ошибка ";
            break;
        case WARN_MSG: prefix = "Проблема "; break;
        case INFO_MSG: prefix = "Инфо "; break;
        case DBG_MSG: prefix = "Отладка "; break;
        }
        msg = prefix+"в файле " + tmpm.file + " строка " + QString::number(tmpm.line);
    }
    if ((msgtype == ER_MSG) || (msgtype == DBG_MSG))
        LogFile.error("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    else if (msgtype == WARN_MSG)
        LogFile.warning("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    else
        LogFile.info("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    tmpm.msg = msg;
    ErMsgPool.append(tmpm);
}

void Error::ShowErMsg(int ermsgnum)
{
    if (ermsgnum < ErrMsgs.size())
        ERMSG(ErrMsgs.at(ermsgnum));
    else
        ERMSG("Произошла неведомая фигня #"+QString::number(ermsgnum,10));
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

