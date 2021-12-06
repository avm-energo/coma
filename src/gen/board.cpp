#include "board.h"

#include "../gen/modules.h"
#include "../gen/registers.h"
#include "error.h"
#include "stdfunc.h"

#include <QDebug>

bool isKnownModule(quint16 mtypeb, quint16 mtypem)
{
    if (Modules::BaseBoards.contains(mtypeb))
    {
        if (mtypeb != Modules::MTM_00)
        {
            return Modules::MezzanineBoards.contains(mtypem);
        }
        return true;
    }
    return false;
}

Board::Board(Singleton::token)
{
    m_interfaceType = Unknown;
    m_connectionState = ConnectionState::Closed;
    m_boardType = Types::None;
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

quint16 Board::type(Board::Types type) const
{
    switch (type)
    {
    case Base:
        return typeB();
    case Mezzanine:
        return typeM();
    case None:
        return 0;
    default:
        return Board::type();
    }
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

Board::InterfaceType Board::interfaceType() const
{
    return m_interfaceType;
}

void Board::setInterfaceType(Board::InterfaceType iface)
{
    m_interfaceType = iface;
    emit interfaceTypeChanged(iface);
}

Board::ConnectionState Board::connectionState() const
{
    return m_connectionState;
}

void Board::setConnectionState(ConnectionState connectionState)
{
    Q_ASSERT(connectionState != m_connectionState);
    if (m_connectionState == connectionState && m_connectionState == ConnectionState::Connected)
        Q_ASSERT("Try to connect while still connected");
    m_connectionState = connectionState;
    emit connectionStateChanged(connectionState);
}

void Board::update(const DataTypes::BitStringStruct &bs)
{
    // Only bsi block
    if (bs.sigAdr < 1 || bs.sigAdr > 15)
        return updateExt(bs);
    quint32 &item = *(reinterpret_cast<quint32 *>(&m_startupInfoBlock) + (bs.sigAdr - Regs::bsiStartReg));

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
    m_interfaceType = Unknown;
    m_connectionState = ConnectionState::Closed;
    m_boardType = Types::None;
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
    constexpr auto lastExt0Reg = Regs::bsiExtStartReg + minCount;
    // BsiExt0
    if ((bs.sigAdr >= Regs::bsiExtStartReg) && (bs.sigAdr <= lastExt0Reg))
    {
        quint32 &item = *(reinterpret_cast<quint32 *>(&m_startupInfoBlockExt) + (bs.sigAdr - Regs::bsiExtStartReg));
        item = bs.sigVal;
        ++m_updateCounterExt;
    }
    if (m_updateCounterExt == minCount)
    {
        emit readyReadExt();
        m_updateCounterExt = 0;
    }
}

bool Board::isUSIO(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    using Modules::BaseBoard;
    using Modules::MezzanineBoard;
    bool status = false;
    switch (typeB)
    {
    case Modules::MTB_21:
    case Modules::MTB_22:
    case Modules::MTB_31:
    case Modules::MTB_33:
    case Modules::MTB_34:
    case Modules::MTB_35:
        status = true;
        break;
    default:
        status = false;
    }
    switch (typeM)
    {
    case Modules::MTM_00:
    case Modules::MTM_21:
    case Modules::MTM_22:
    case Modules::MTM_31:
    case Modules::MTM_33:
    case Modules::MTM_34:
    case Modules::MTM_35:
        status &= true;
        break;
    default:
        status = false;
    }
    return status;
}

Board::Types Board::boardType() const
{
    return m_boardType;
}

void Board::setBoardType(const Types &boardType)
{
    m_boardType = boardType;
    emit boardTypeChanged(boardType);
}

Board::DeviceType Board::deviceType() const
{
    return m_deviceType;
}

void Board::setDeviceType(const DeviceType &deviceType)
{
    m_deviceType = deviceType;
    emit deviceTypeChanged(deviceType);
}
