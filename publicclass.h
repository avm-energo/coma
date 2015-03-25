#ifndef PUBLICCLASS_H
#define PUBLICCLASS_H

#define MT_C    1
#define MT_D    2
#define MT_A    3
#define MT_E    4

#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include "serialthread.h"

class publicclass
{
public:
    publicclass();

    QSerialPort serial;
    qint32 MType, MType1;
    QStringList MTypes;
    SerialThread *SThread;

    QString VerToStr(qint32);
    qint32 ANumD();
    qint32 ANumCh1();
    qint32 ANumCh2();
    qint32 ATyp1();
    qint32 ATyp2();
    qint32 AMdf();
};

extern publicclass pc;

#endif // PUBLICCLASS_H
