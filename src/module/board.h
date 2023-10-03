#pragma once

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

#include "modules.h"

#include <gen/datatypes.h>
#include <gen/singleton.h>
#include <interfaces/types/common_types.h>

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

    // TODO: Здесь мог бы быть ваш рефакторинг ;)
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
    Q_ENUM(Types)
    Q_ENUM(ConnectionState)

    Q_PROPERTY(Interface::IfaceType interface READ interfaceType WRITE setInterfaceType NOTIFY interfaceTypeChanged)
    Q_PROPERTY(DeviceType device READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
    Q_PROPERTY(Types board READ boardType WRITE setBoardType NOTIFY boardTypeChanged)
    Q_PROPERTY(ConnectionState connection READ connectionState WRITE setConnectionState NOTIFY connectionStateChanged)

    explicit Board(token);

    quint16 typeB() const;
    quint16 typeM() const;
    quint16 type() const;

    bool isCrcValid() const
    {
        return m_startupInfoBlock.Cfcrc;
    }

    QString moduleName() const;

    quint32 serialNumber(Types type) const;
    QString UID() const;

    Interface::IfaceType interfaceType() const;
    void setInterfaceType(Interface::IfaceType iface);

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

    static bool isUSIO(Modules::BaseBoard &typeB, Modules::MezzanineBoard &typeM);

private:
    static constexpr int StartupInfoBlockMembers = sizeof(Modules::StartupInfoBlock) / sizeof(quint32);
    Interface::IfaceType m_interfaceType;
    DeviceType m_deviceType;
    Types m_boardType;
    ConnectionState m_connectionState;

    Modules::StartupInfoBlock m_startupInfoBlock {};
    Modules::StartupInfoBlockExt0 m_startupInfoBlockExt {};

    void updateExt(const DataTypes::BitStringStruct &bs);

    int m_updateCounter = 0;
    int m_updateCounterExt = 0;
    bool m_updateType = false;

signals:
    void interfaceTypeChanged(Interface::IfaceType);
    void deviceTypeChanged(Board::DeviceType);
    void boardTypeChanged(Board::Types);
    void typeChanged();
    void typeChanged(quint16);
    void connectionStateChanged(Board::ConnectionState);
    /// \brief This signal is emitted when StartupInfoBlock::Hth is changed.
    void healthChanged(quint32);
    /// \brief This signal is emitted when all StartupInfoBlock members are updated.
    void readyRead();
    /// \brief This signal is emitted when all StartupInfoBlockExt members are updated.
    void readyReadExt();
};

// Q_DECLARE_METATYPE(Board::InterfaceType)
