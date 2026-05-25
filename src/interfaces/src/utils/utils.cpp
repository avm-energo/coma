#include "interfaces/utils/utils.h"

namespace SerialUtils
{
QString braudToString(QSerialPort::BaudRate rate)
{
    switch (rate)
    {
    case QSerialPort::Baud1200:    return QStringLiteral("1200");
    case QSerialPort::Baud2400:    return QStringLiteral("2400");
    case QSerialPort::Baud4800:    return QStringLiteral("4800");
    case QSerialPort::Baud9600:    return QStringLiteral("9600");
    case QSerialPort::Baud19200:   return QStringLiteral("19200");
    case QSerialPort::Baud38400:   return QStringLiteral("38400");
    case QSerialPort::Baud57600:   return QStringLiteral("57600");
    case QSerialPort::Baud115200:  return QStringLiteral("115200");
    default:                       return {};
    }
}

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
