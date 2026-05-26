#include "interfaces/utils/utils.h"

namespace SerialUtils
{
QString stopBitsToString(QSerialPort::StopBits bit)
{
    switch (bit)
    {
    case QSerialPort::StopBits::OneStop: return QStringLiteral("1");
    case QSerialPort::StopBits::TwoStop: return QStringLiteral("2");
    default:                             return {};
    }
}

const QStringList& allBaudValuesRS485()
{
    static const QStringList list = {
        "2400", "4800", "9600", "19200", "38400", "57600", "115200"
    };

    return list;
}
}
