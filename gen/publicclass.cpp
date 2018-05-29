#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include "publicclass.h"
#include "log.h"

publicclass pc;

publicclass::publicclass()
{
    Emul = false;
    ModuleBsi.MTypeB = ModuleBsi.MTypeM = 0xFFFFFFFF;

    SystemHomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"+PROGNAME+"/";
    QDir dir(SystemHomeDir);
    if (!dir.exists())
        dir.mkdir(SystemHomeDir);
    QFile file;
    QString ermsgspath = SystemHomeDir;
    log.Init(SystemHomeDir + LOGFILE);
    log.info("=== Log started ===\n");
    file.setFileName(ermsgspath+"ermsgs.dat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        result = ER_FILEOPENERROR;
        return;
    }
    else
    {
        QString tmpString;
        QTextStream streamfile(&file);
        streamfile.setCodec("WIN-1251");
        do
        {
            tmpString = streamfile.readLine();
            if (tmpString.isEmpty())
                errmsgs << "";
            else if (tmpString.at(0) != '#')
                errmsgs << tmpString;
        } while (!streamfile.atEnd());
    }
    result = NOERROR;
}

QString publicclass::VerToStr(quint32 num)
{
    int mv = (num&0xFF000000)>>24;
    int lv = (num&0x00FF0000)>>16;
    int sv = (num&0x0000FFFF);
    QString tmpString = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 16, QChar('0'));
    return tmpString;
}

QString publicclass::NsTimeToString(quint64 nstime)
{
    quint32 unixtime = nstime / 1000000000L; // unix время
    QString tmps = QDateTime::fromTime_t(unixtime).toString("dd-MM-yyyy hh:mm:ss");
    quint32 nst = nstime % 1000000000L; // оставшееся время в нс до секунды
    tmps += "." + QString("%1").arg((nst/1000000L),3,10,QChar('0'));
    nst = nst % 1000000L;
    tmps += "." + QString("%1").arg((nst/1000L),3,10,QChar('0'));
    nst = nst % 1000L;
    tmps += "." + QString("%1").arg(nst,3,10,QChar('0'));
    return tmps;
}

void publicclass::AddErrMsg(ermsgtype msgtype, QString file, int line, QString msg)
{
    if (ErMsgPool.size()>=ER_BUFMAX)
        ErMsgPool.removeFirst();
    ermsg tmpm;
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
        log.error("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    else if (msgtype == WARN_MSG)
        log.warning("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    else
        log.info("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    tmpm.msg = msg;
    ErMsgPool.append(tmpm);
}

QString publicclass::ChooseFileForOpen(QWidget *parent, QString mask)
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getOpenFileName(parent, "Открыть файл", HomeDir, mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    pc.HomeDir = info.absolutePath();
    dlg->close();
    return filename;
}

int publicclass::LoadFromFile(const QString &filename, QByteArray &ba)
{
    if (filename.isEmpty())
    {
        ERMSG("Пустое имя файла");
        return ER_FILEOPEN; // Пустое имя файла
    }
    QFile *file = new QFile;
    file->setFileName(filename);
    if (!file->open(QIODevice::ReadOnly))
    {
        ERMSG("Ошибка открытия файла");
        return ER_FILEOPEN; // Ошибка открытия файла
    }
    ba = file->readAll();
    file->close();
    return NOERROR;
}

// Input: QString mask: описание файлов, например: "Файлы журналов (*.swj)"; QString ext - расширение по умолчанию
// Output: QString filename

QString publicclass::ChooseFileForSave(QWidget *parent, const QString &mask, const QString &ext)
{
    QString MTypeM = (ModuleBsi.MTypeM == 0) ? "00" : QString::number(ModuleBsi.MTypeM, 16);
    QString tmps = HomeDir + "/" + QString::number(ModuleBsi.MTypeB, 16)+MTypeM+"-"+\
            QString("%1").arg(ModuleBsi.SerialNum, 8, 10, QChar('0'))+"."+ext;
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getSaveFileName(parent, "Сохранить файл", tmps, mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    pc.HomeDir = info.absolutePath();
    dlg->close();
    return filename;
}

int publicclass::SaveToFile(const QString &filename, QByteArray &src, unsigned int numbytes)
{
    if (filename.isEmpty())
        return ER_FILENAMEEMP; // Пустое имя файла
    QFile *file = new QFile;
    file->setFileName(filename);
    if (!file->open(QIODevice::WriteOnly))
        return ER_FILEOPEN; // Ошибка открытия файла
    int res = file->write(src, numbytes);
    file->close();
    delete file;
    if (res == GENERALERROR)
        return ER_FILEWRITE; // ошибка записи
    return NOERROR; // нет ошибок
}

bool publicclass::FloatInRange(float var, float value)
{
    if ((var > (value-TH01)) && (var < (value+TH01)))
        return true;
    else
        return false;
}

QString publicclass::UnixTime64ToString(quint64 utime)
{
    quint32 tmpi = utime >> 32;
    QDateTime tn = QDateTime::fromTime_t(tmpi, Qt::UTC);
    quint32 timens = utime & 0xFFFFFFFF;
    QString ms = QString::number((timens/1000000));
    QString mcs = QString::number(((timens-ms.toInt()*1000000)/1000));
    QString ns = QString::number(timens-ms.toInt()*1000000-mcs.toInt()*1000);
    QString time = tn.toString("dd/MM/yyyy hh:mm:ss.")+ms+"."+mcs+"."+ns;
    return time;
}

int publicclass::IndexByBit(quint32 dword)
{
    quint32 bit = 0x00000001;
    for (int i=0; i<32; ++i)
        if (dword & bit)
            return (i+1);
    return 0;
}

quint32 publicclass::BitByIndex(int idx)
{
    if ((idx == 0) || (idx > 31))
        return 0;
    return (0x00000001 << (idx-1));
}

void publicclass::ErMsg(int ermsgnum)
{
    if (ermsgnum < pc.errmsgs.size())
        ERMSG(pc.errmsgs.at(ermsgnum));
    else
        ERMSG("Произошла неведомая фигня #"+QString::number(ermsgnum,10));
}
