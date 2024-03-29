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

enum ExceptionCode : quint8
{
    IllegalFunction = 0x01,
    IllegalDataAddress = 0x02,
    IllegalDataValue = 0x03,
    ServerDeviceFailure = 0x04,
    Acknowledge = 0x05,
    ServerDeviceBusy = 0x06,
    MemoryParityError = 0x08,
    GatewayPathUnavailable = 0x0A,
    GatewayTargetDeviceFailedToRespond = 0x0B
};
Q_ENUM_NS(ExceptionCode)

struct Request
{
    FunctionCode code;
    std::uint16_t address;
    std::uint16_t quantity;
    bool isLastSection = false; // using only for writing files
    QByteArray data;
};

constexpr quint16 firmwareAddr = 802;
constexpr quint16 typeOscAddr = 813;
constexpr quint16 timeReg = 4600;
constexpr quint16 enableWriteHwAddr = 40001;
constexpr quint16 hardwareVerAddr = 40010;
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
