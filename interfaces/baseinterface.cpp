#include "baseinterface.h"

#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"

#include <QCoreApplication>

BaseInterface::BaseInterface(QObject *parent) : QObject(parent), m_working(false), Log(new LogClass(this))
{
}

void BaseInterface::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    // NOTE Избежать сужающих кастов
    DataTypes::Signal signal { static_cast<quint16>(sigAdr), static_cast<quint16>(sigCount) };
    writeCommand(Queries::QC_ReqAlarms, QVariant::fromValue(signal));
}

void BaseInterface::reqBlockSync(quint32 blocknum, DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    m_busy = true;
    connect(&DataManager::GetInstance(), &DataManager::blockReceived, this, &BaseInterface::resultReady);
    switch (blocktype)
    {
    case DataBlockTypes::BacBlock:
        writeCommand(Queries::QUSB_ReqTuningCoef);
    }

    writeCommand(Queries::QC_, item);
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
}

void BaseInterface::resultReady(const DataTypes::BlockStruct &result)
{
    disconnect(&DataManager::GetInstance(), &DataManager::blockReceived, this, &BaseInterface::resultReady);
    m_busy = false;
}
