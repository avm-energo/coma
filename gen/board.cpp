#include "board.h"
Board::Board(Singleton::token)
{
    m_typeB = 0;
    m_typeM = 0;
    m_interfaceType = Unknown;
    m_connectionState = ConnectionState::Closed;
    m_boardType = BoardType::NONE;
    m_deviceType = DeviceType::Module;
    //    Q_UNUSED(parent)
}

quint16 Board::typeB() const
{
    return m_typeB;
}

void Board::setTypeB(const quint16 &typeB)
{
    m_typeB = typeB;
    emit typeChanged();
}

quint16 Board::typeM() const
{
    return m_typeM;
}

void Board::setTypeM(const quint16 &typeM)
{
    m_typeM = typeM;
    emit typeChanged();
}

quint16 Board::type() const
{
    return quint16((typeB() << 8) + typeM());
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
    if (m_connectionState == connectionState && m_connectionState == ConnectionState::Connected)
        Q_ASSERT("Try to connect while still connected");
    m_connectionState = connectionState;
    emit connectionStateChanged(connectionState);
}

Board::BoardType Board::boardType() const
{
    return m_boardType;
}

void Board::setBoardType(const BoardType &boardType)
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
