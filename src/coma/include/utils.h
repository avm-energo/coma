#pragma once

#include <QString>
#include <QStringList>
#include <QSerialPort>

namespace Utils
{
QString braudToString(QSerialPort::BaudRate rate);
const QStringList& allBaudValuesRS485();

QString stopBitsToString(QSerialPort::StopBits bit);
}
