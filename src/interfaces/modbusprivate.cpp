#include "modbusprivate.h"

QDebug MBS::operator<<(QDebug debug, const MBS::CommandStruct &cmd)
{
    debug.nospace() << cmd.cmd << ":" << cmd.adr << ":" << cmd.quantity << ":";
    for (const quint8 byte : cmd.data)
        debug.nospace() << QString::number(byte, 16);

    return debug.maybeSpace();
}
