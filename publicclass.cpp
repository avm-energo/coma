#include "publicclass.h"

publicclass pc;

publicclass::publicclass()
{
    MTypes.append("Ц");
    MTypes.append("Д");
    MTypes.append("А");
    MTypes.append("Э");
    MType = MType1 = 0xFFFFFFFF;
}

QString publicclass::VerToStr(qint32 num)
{
    int mv = (num&0xFF000000)>>24;
    int lv = (num&0x00FF0000)>>16;
    int sv = (num&0x0000FFFF);
    QString tmpString = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 16, QChar('0'));
    return tmpString;
}


qint32 publicclass::ANumD()
{
    qint32 tmpint = (MType1 & 0xF0000000) >> 28;
    return tmpint;
}

qint32 publicclass::ANumCh1()
{
    qint32 tmpint = (MType1 & 0x00F80000) >> 19;
    return tmpint;
}

qint32 publicclass::ANumCh2()
{
    qint32 tmpint = (MType1 & 0x0000F800) >> 11;
    return tmpint;
}

qint32 publicclass::ATyp1()
{
    qint32 tmpint = (MType1 & 0x00070000) >> 16;
    return tmpint;
}

qint32 publicclass::ATyp2()
{
    qint32 tmpint = (MType1 & 0x00000700) >> 8;
    return tmpint;
}

qint32 publicclass::AMdf()
{
    qint32 tmpint = (MType1 & 0x000000FF);
    return tmpint;
}
