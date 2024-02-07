#include "board.h"

#include "modules.h"

#include <QDebug>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <interfaces/conn/async_connection.h>

using namespace Interface;

Board::Board(Singleton::token)
{
    m_interfaceType = Interface::IfaceType::Unknown;
}

quint16 Board::typeB() const
{
    return m_startupInfoBlock.MTypeB;
}

quint16 Board::typeM() const
{
    return m_startupInfoBlock.MTypeM;
}

quint16 Board::type() const
{
    const quint16 Mtypem = typeM();
    const quint16 Mtypeb = typeB() << 8;
    return quint16(Mtypeb + Mtypem);
}

QString Board::moduleName() const
{
    QString name = QVariant::fromValue(Modules::Model(type())).toString();
    if (name.isEmpty())
        name = Modules::BaseBoards.value(typeB()) + Modules::MezzanineBoards.value(typeM());
    return name;
}

quint32 Board::serialNumber(Board::Types type) const
{
    switch (type)
    {
    case Base:
        return m_startupInfoBlock.SerialNumB;
    case Mezzanine:
        return m_startupInfoBlock.SerialNumM;
    case None:
        return 0;
    default:
        return m_startupInfoBlock.SerialNum;
    }
}

QString Board::UID() const
{
    return QString::number(m_startupInfoBlock.UIDHigh, 16) + QString::number(m_startupInfoBlock.UIDMid, 16)
        + QString::number(m_startupInfoBlock.UIDLow, 16);
}

Interface::IfaceType Board::interfaceType() const
{
    return m_interfaceType;
}

void Board::setInterfaceType(Interface::IfaceType iface)
{
    m_interfaceType = iface;
    emit interfaceTypeChanged(iface);
}

void Board::update(const DataTypes::BitStringStruct &bs)
{
    // Only bsi block
    if (bs.sigAdr < 1 || bs.sigAdr > 15)
        return updateExt(bs);
    quint32 &item = *(reinterpret_cast<quint32 *>(&m_startupInfoBlock) + (bs.sigAdr - addr::bsiStartReg));

    item = bs.sigVal;
    m_updateCounter++;
    // Last value updated
    if (&item == &m_startupInfoBlock.Hth)
    {
        emit healthChanged(m_startupInfoBlock.Hth);
    }
    else if (&item == &m_startupInfoBlock.MTypeB || &item == &m_startupInfoBlock.MTypeM)
    {
        if (!m_updateType)
            m_updateType = true;
        else
        {
            m_updateType = false;
            emit typeChanged(type());
            emit typeChanged();
        }
    }

    if (m_updateCounter == StartupInfoBlockMembers)
    {
        emit readyRead();
        m_updateCounter = 0;
    }
}

void Board::reset()
{
    m_interfaceType = Interface::IfaceType::Unknown;
    m_startupInfoBlock = {};
    m_startupInfoBlockExt = {};
}

quint32 Board::health() const
{
    return m_startupInfoBlock.Hth;
}

bool Board::noConfig() const
{
    return (health() & HTH_CONFIG);
}

bool Board::noRegPars() const
{
    return health() & HTH_REGPARS;
}

void Board::updateExt(const DataTypes::BitStringStruct &bs)
{
    constexpr auto minCount = sizeof(Modules::StartupInfoBlockExt0) / sizeof(quint32);
    constexpr auto lastExt0Reg = addr::bsiExtStartReg + minCount;
    // BsiExt0
    if ((bs.sigAdr >= addr::bsiExtStartReg) && (bs.sigAdr <= lastExt0Reg))
    {
        quint32 &item = *(reinterpret_cast<quint32 *>(&m_startupInfoBlockExt) + (bs.sigAdr - addr::bsiExtStartReg));
        item = bs.sigVal;
        ++m_updateCounterExt;
    }
    if (m_updateCounterExt == minCount)
    {
        emit readyReadExt();
        m_updateCounterExt = 0;
    }
}
