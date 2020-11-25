#pragma once

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

#include "../module/modules.h"
#include "datatypes.h"
#include "singleton.h"

class Board : public QObject, public Singleton<Board>
{
    Q_OBJECT

public:
    enum DeviceType
    {
        Module,
        Device
    };

    enum Range
    {
        High,
        Mid,
        Low
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

    enum Types
    {
        None = 0,
        Base = 1,
        Mezzanine = 2,
        BaseMezz = 3,
        Additional = 4,
        BaseAdd = 5,
        MezzAdd = 6,
        BaseMezzAdd = 7
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
    Q_ENUM(Types)
    Q_ENUM(ConnectionState)

    Q_PROPERTY(InterfaceType interface READ interfaceType WRITE setInterfaceType NOTIFY interfaceTypeChanged)
    Q_PROPERTY(DeviceType device READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
    Q_PROPERTY(Types board READ boardType WRITE setBoardType NOTIFY boardTypeChanged)
    Q_PROPERTY(ConnectionState connection READ connectionState WRITE setConnectionState NOTIFY connectionStateChanged)

    explicit Board(token);

    quint16 typeB() const;
    // void setTypeB(const quint16 &typeB);

    quint16 typeM() const;
    // void setTypeM(const quint16 &typeM);

    quint16 type() const;
    quint16 type(Types type) const;

    quint32 serialNumber(Types type) const;
    QString UID() const;

    InterfaceType interfaceType() const;
    void setInterfaceType(InterfaceType iface);

    DeviceType deviceType() const;
    void setDeviceType(const DeviceType &deviceType);

    Types boardType() const;
    void setBoardType(const Types &boardType);

    ConnectionState connectionState() const;
    void setConnectionState(ConnectionState connectionState);

    // QList<quint16> getBaseBoardsList() const;

    void update(const DataTypes::BitStringStruct &bs);

    quint32 health() const;
    bool noConfig() const;
    bool noRegPars() const;

    Modules::StartupInfoBlock baseSerialInfo() const;

private:
    InterfaceType m_interfaceType;
    DeviceType m_deviceType;
    Types m_boardType;
    ConnectionState m_connectionState;

    Modules::StartupInfoBlock m_baseSerialInfo {};
    Modules::StartupInfoBlockExt m_baseSerialInfoExt {};

    template <typename T> bool isSerialNumberSet(T value)
    {
        return value == 0xFFFFFFFF;
    }

    template <typename T, typename... Types> bool isSerialNumberSet(T value, Types... args)
    {
        return value || isSerialNumberSet(args...);
    }

signals:
    void interfaceTypeChanged(Board::InterfaceType);
    void deviceTypeChanged(Board::DeviceType);
    void boardTypeChanged(Board::Types);
    void typeChanged();
    void connectionStateChanged(Board::ConnectionState);
    void healthChanged(quint32);
    void readyRead();
};
