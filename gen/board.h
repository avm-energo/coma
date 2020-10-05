#pragma once

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

#include "singleton.h"

#include <QObject>

class Board final : public QObject, public Singleton<Board>
{
    Q_OBJECT

public:
    enum DeviceType
    {
        Module,
        Device
    };

    /**
     *  Перечисление для хранения списка интерфейсов
     */
    enum InterfaceType
    {
        Unknown,
        USB,
        Ethernet,
        RS485
    };

    enum BoardType
    {
        NONE = 0,
        BASE = 1,
        MEZONIN = 2,
        BSMZ = 3,
        MODULE = 4
    };
    /**
     *  Перечисление для хранения списка приборов
     */
    enum DeviceModel
    {
        KTF = 0xA287,
        KIV = 0xA284,
        KDV = 0xA387
    };
    /**
     *  Перечисление для хранения текущего состояния подключения
     */
    enum class ConnectionState
    {
        Connected,
        Closed,
        AboutToFinish
    };

    Q_ENUM(DeviceType)
    Q_ENUM(InterfaceType)
    Q_ENUM(BoardType)
    Q_ENUM(DeviceModel)
    Q_ENUM(ConnectionState)

    Q_PROPERTY(InterfaceType interface READ interfaceType WRITE setInterfaceType NOTIFY interfaceTypeChanged)
    Q_PROPERTY(DeviceType device READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
    Q_PROPERTY(BoardType board READ boardType WRITE setBoardType NOTIFY boardTypeChanged)
    Q_PROPERTY(ConnectionState connection READ connectionState WRITE setConnectionState NOTIFY connectionStateChanged)

    explicit Board(token);

    quint16 typeB() const;
    void setTypeB(const quint16 &typeB);

    quint16 typeM() const;
    void setTypeM(const quint16 &typeM);

    quint16 type() const;

    InterfaceType interfaceType() const;
    void setInterfaceType(InterfaceType iface);

    DeviceType deviceType() const;
    void setDeviceType(const DeviceType &deviceType);

    BoardType boardType() const;
    void setBoardType(const BoardType &boardType);

    ConnectionState connectionState() const;
    void setConnectionState(ConnectionState connectionState);

private:
    InterfaceType m_interfaceType;
    DeviceType m_deviceType;
    BoardType m_boardType;
    ConnectionState m_connectionState;

    quint16 m_typeB;
    quint16 m_typeM;
signals:
    void interfaceTypeChanged(Board::InterfaceType);
    void deviceTypeChanged(Board::DeviceType);
    void boardTypeChanged(Board::BoardType);
    void typeChanged();
    void connectionStateChanged(Board::ConnectionState);
};
