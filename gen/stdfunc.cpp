#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include "stdfunc.h"
#include "maindef.h"

QString StdFunc::HomeDir = ""; // рабочий каталог программы
QString StdFunc::SystemHomeDir = ""; // системный каталог программы
bool StdFunc::Emul = false;
bool StdFunc::Cancelled = false;
QString StdFunc::PrbMsg = "";
QString StdFunc::MIPIP = "";

StdFunc::StdFunc()
{
    Emul = false;

    SystemHomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"+PROGNAME+"/";
    QDir dir(SystemHomeDir);
    if (!dir.exists())
        dir.mkdir(SystemHomeDir);
}

QString StdFunc::VerToStr(quint32 num)
{
    int mv = (num&0xFF000000)>>24;
    int lv = (num&0x00FF0000)>>16;
    int sv = (num&0x0000FFFF);
    QString tmpString = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 16, QChar('0'));
    return tmpString;
}

bool StdFunc::FloatInRange(float var, float value)
{
    if ((var > (value-TH01)) && (var < (value+TH01)))
        return true;
    else
        return false;
}

void StdFunc::SetHomeDir(const QString &dir)
{
    HomeDir = dir;
}

QString StdFunc::GetHomeDir()
{
    return HomeDir;
}

QString StdFunc::GetSystemHomeDir()
{
    return SystemHomeDir;
}

void StdFunc::Cancel()
{
    Cancelled = true;
}

void StdFunc::ClearCancel()
{
    Cancelled = false;
}

bool StdFunc::IsCancelled()
{
    return Cancelled;
}

bool StdFunc::IsInEmulateMode()
{
    return Emul;
}

void StdFunc::SetEmulated(bool tb)
{
    Emul = tb;
}

int StdFunc::IndexByBit(quint32 dword)
{
    quint32 bit = 0x00000001;
    for (int i=0; i<32; ++i)
        if (dword & bit)
            return (i+1);
    return 0;
}

quint32 StdFunc::BitByIndex(int idx)
{
    if ((idx == 0) || (idx > 31))
        return 0;
    return (0x00000001 << (idx-1));
}

QString StdFunc::PrbMessage()
{
    return PrbMsg;
}

void StdFunc::SetPrbMessage(const QString &msg)
{
    PrbMsg = msg;
}
