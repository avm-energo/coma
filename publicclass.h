#ifndef PUBLICCLASS_H
#define PUBLICCLASS_H

#define MT_C    1
#define MT_D    2
#define MT_A    3
#define MT_E    4

#include <QtSerialPort/QSerialPort>

class publicclass
{
public:
    publicclass();

    QSerialPort serial;
    int MType;
};

extern publicclass pc;

#endif // PUBLICCLASS_H
