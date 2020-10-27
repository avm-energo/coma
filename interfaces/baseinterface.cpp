#include "baseinterface.h"

BaseInterface::BaseInterface(QObject *parent) : QObject(parent) { }

void BaseInterface::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    QList<DataTypes::SignalsStruct> list;
    DataTypes::SignalsStruct sig { DataTypes::SignalTypes::BitString, sigAdr };
    list.append(sig);
    sig = { DataTypes::SignalTypes::BitString, sigCount };
    list.append(sig);
    writeCommand(Queries::QC_ReqAlarms, list);
}
