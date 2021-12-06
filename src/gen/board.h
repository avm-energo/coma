#pragma once

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

#include "../gen/modules.h"
#include "datatypes.h"
#include "singleton.h"

class Board : public QObject, public Singleton<Board>
{
    Q_OBJECT

public:
    enum DeviceType
    {
        Controller,
        Module
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
        RS485,
        Emulator
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

    quint16 typeM() const;

    quint16 type() const;
    quint16 type(Types type) const;

    bool isCrcValid() const
    {
        return m_startupInfoBlock.Cfcrc;
    }

    QString moduleName() const;

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

    void update(const DataTypes::BitStringStruct &bs);
    void reset();

    quint32 health() const;
    bool noConfig() const;
    bool noRegPars() const;

    const Modules::StartupInfoBlock &baseSerialInfo() const
    {
        return m_startupInfoBlock;
    }
    const Modules::StartupInfoBlockExt0 &baseSerialInfoExt() const
    {
        return m_startupInfoBlockExt;
    }

    static bool isUSIO(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM);

private:
    static constexpr int StartupInfoBlockMembers = sizeof(Modules::StartupInfoBlock) / sizeof(quint32);
    InterfaceType m_interfaceType;
    DeviceType m_deviceType;
    Types m_boardType;
    ConnectionState m_connectionState;

    Modules::StartupInfoBlock m_startupInfoBlock {};
    Modules::StartupInfoBlockExt0 m_startupInfoBlockExt {};

    void updateExt(const DataTypes::BitStringStruct &bs);

    template <typename T> bool isSerialNumberSet(T value)
    {
        return value == 0xFFFFFFFF;
    }

    template <typename T, typename... Types> bool isSerialNumberSet(T value, Types... args)
    {
        return value || isSerialNumberSet(args...);
    }
    int m_updateCounter = 0;
    int m_updateCounterExt = 0;
    bool m_updateType = false;

signals:
    void interfaceTypeChanged(Board::InterfaceType);
    void deviceTypeChanged(Board::DeviceType);
    void boardTypeChanged(Board::Types);
    void typeChanged();
    void typeChanged(quint16);
    void connectionStateChanged(Board::ConnectionState);
    /// This signal is emitted when StartupInfoBlock::Hth
    void healthChanged(quint32);
    /// This signal is emitted when all StartupInfoBlock members updated
    void readyRead();
    /// This signal is emitted when all StartupInfoBlockExt members updated
    void readyReadExt();
};
