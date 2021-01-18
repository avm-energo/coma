#include "modbusprivate.h"

QDebug CommandsMBS::operator<<(QDebug debug, const CommandsMBS::ModbusGroup &settings)
{
    debug.nospace() << settings.id << ":" << settings.function << ":"
                    << QVariant::fromValue(settings.dataType).toString() << ":" << settings.startAddr << ":"
                    << settings.count;
    return debug.maybeSpace();
}

QDebug CommandsMBS::operator<<(QDebug debug, const CommandsMBS::CommandStruct &cmd)
{
    debug.nospace() << cmd.cmd << ":" << cmd.adr << ":" << cmd.quantity << ":"
                    << QVariant::fromValue(cmd.type).toString() << ":" << cmd.sender << ":";
    for (const quint8 byte : cmd.data)
        debug.nospace() << QString::number(byte, 16);

    return debug.maybeSpace();
}
