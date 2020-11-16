#include "baseinterface.h"

BaseInterface::BaseInterface(QObject *parent) : QObject(parent), m_working(false), Log(new LogClass(this))
{
}

void BaseInterface::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    // NOTE Избежать сужающих кастов
    DataTypes::Signal signal { static_cast<quint16>(sigAdr), static_cast<quint16>(sigCount) };
    writeCommand(Queries::QC_ReqAlarms, QVariant::fromValue(signal));
}
