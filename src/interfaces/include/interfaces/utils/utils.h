#pragma once

#include <QString>
#include <QStringList>
#include <QSerialPort>

namespace SerialUtils
{
QString baudToString(QSerialPort::BaudRate rate);
const QStringList& allBaudValuesRS485();

QString stopBitsToString(QSerialPort::StopBits bit);
}
