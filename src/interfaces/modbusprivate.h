#pragma once

#include <QDebug>
#include <QObject>
#include <QVariant>

namespace MBS
{
Q_NAMESPACE

enum Commands : quint8
{
    ReadCoils = 0x01,
    ReadDiscreteInputs = 0x02,
    ReadHoldingRegisters = 0x03,
    ReadInputRegister = 0x04,
    WriteMultipleRegisters = 0x10,
    ReadFileSection = 0x41,
    WriteFileSection = 0x42
};
Q_ENUM_NS(Commands)

struct CommandStruct
{
    Commands cmd;
    quint16 adr;
    quint16 quantity;
    QByteArray data;
};

constexpr quint8 FileSectionLength = 0xF6;

QDebug operator<<(QDebug debug, const MBS::CommandStruct &cmd);
}
Q_DECLARE_METATYPE(MBS::CommandStruct)
