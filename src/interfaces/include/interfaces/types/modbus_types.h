#pragma once

#include <QDebug>
#include <QtEndian>
#include <gen/stdfunc.h>

namespace Modbus
{
Q_NAMESPACE

enum FunctionCode : quint8
{
    ReadCoils = 0x01,
    ReadDiscreteInputs = 0x02,
    ReadHoldingRegisters = 0x03,
    ReadInputRegister = 0x04,
    WriteMultipleRegisters = 0x10,
    ReadFileSection = 0x41,
    WriteFileSection = 0x42
};
Q_ENUM_NS(FunctionCode)

struct Request
{
    FunctionCode code;
    std::uint16_t address;
    std::uint16_t quantity;
    bool isLastSection = false; // using only for writing files
    QByteArray data;
};

constexpr quint16 firmwareModbusAddr = 802;
constexpr quint16 timeReg = 4600;
constexpr quint8 fileSectionLength = 0xF6;

inline QDebug operator<<(QDebug debug, const Modbus::Request &cmd)
{
    debug.nospace() << cmd.code << ":" << cmd.address << ":" << cmd.quantity << ":";
    for (const quint8 byte : cmd.data)
        debug.nospace() << QString::number(byte, 16);
    return debug.maybeSpace();
}

}
Q_DECLARE_METATYPE(Modbus::Request)
