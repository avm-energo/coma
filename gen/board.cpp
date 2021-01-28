#include "board.h"

#include "../module/modules.h"
#include "../module/registers.h"
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
    m_deviceType = DeviceType::Module;
    //    Q_UNUSED(parent)
}

quint16 Board::typeB() const
{
    return m_startupInfoBlock.MTypeB;
}

// void Board::setTypeB(const quint16 &typeB)
//{
//    m_typeB = typeB;
//    emit typeChanged();
//}

quint16 Board::typeM() const
{
    return m_startupInfoBlock.MTypeM;
}

// void Board::setTypeM(const quint16 &typeM)
//{
//    m_typeM = typeM;
//    emit typeChanged();
//}

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
    return QVariant::fromValue(Modules::Model(type())).toString();
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
    //    switch (ran)
    //    {
    //    case High:
    //        return m_baseSerialInfo.UIDHigh;
    //    case Mid:
    //        return m_baseSerialInfo.UIDMid;
    //    case Low:
    //        return m_baseSerialInfo.UIDLow;
    //    default:
    //        return 0;
    //    }
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
    // NOTE Необходимо сделать проверку: пришел ли это сигнал с
    // нужным нам адресом(наш сигнал) или нет - чужие данные
    // Only bsi block
    if (bs.sigAdr < 1 || bs.sigAdr > 15)
        return;
    quint32 &item = *(reinterpret_cast<quint32 *>(&m_startupInfoBlock) + (bs.sigAdr - BSIREG));
    // std::copy_n(&bs.sigVal, sizeof(quint32), &item);
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

quint32 Board::health() const
{
    return m_startupInfoBlock.Hth;
}

bool Board::noConfig() const
{
    return (health() & HTH_CONFIG) || StdFunc::IsInEmulateMode();
}

bool Board::noRegPars() const
{
    return health() & HTH_REGPARS;
}

Modules::StartupInfoBlock Board::baseSerialInfo() const
{
    return m_startupInfoBlock;
}

// QList<quint16> Board::getBaseBoardsList() const
//{
//    QList<quint16> list = Modules::BaseBoards.keys();
//    return list;
//}

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
