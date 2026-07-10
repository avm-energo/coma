#pragma once

#include <QString>
#include <QStringList>
#include <QSerialPort>

namespace SerialUtils
{
const QStringList& allBaudValuesRS485();
QString stopBitsToString(QSerialPort::StopBits bit);
}
