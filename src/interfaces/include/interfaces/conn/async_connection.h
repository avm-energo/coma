#pragma once

#include <interfaces/types/protocol_settings.h>
#include <interfaces/utils/request_queue.h>
#include <interfaces/utils/slot_trait.h>

namespace Interface
{

namespace addr
{
    constexpr quint16 bsiStartReg = 1;
    constexpr quint16 bsiCountRegs = 15;
    constexpr quint16 bsiExtStartReg = 40;
    constexpr quint16 bsiExtCountRegs = 5;
    constexpr quint16 timeReg = 4600;
}

class AsyncConnection final : public QObject
{
    Q_OBJECT
private:
    using FileFormat = DataTypes::FileFormat;
    RequestQueue m_queue;
    IfaceType m_ifaceType;
    State m_connectionState;
    quint32 m_timeout;

public:
    explicit AsyncConnection(QObject *parent = nullptr);
    RequestQueue &getQueue() noexcept;
    void updateProtocol(const ProtocolDescription &desc) noexcept;

    IfaceType getInterfaceType() const noexcept;
    State getConnectionState() const noexcept;
    quint32 getTimeout() const noexcept;
    void setTimeout(const quint32 timeout) noexcept;

    // commands to send
    void reqStartup(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqBSI();
    void reqBSIExt();
    void reqFile(quint32 id, FileFormat format = FileFormat::Binary, quint32 expectedSize = 0);
    void writeFile(quint32 id, const QByteArray &ba);
    void writeConfiguration(const QByteArray &ba);
    void writeFirmware(const QByteArray &ba);
    void reqTime();
    void writeTime(quint32 time);
#ifdef Q_OS_LINUX
    void writeTime(const timespec &time);
#endif
    void writeCommand(Commands cmd, QVariant value = 0);
    void writeCommand(Commands cmd, const QVariantList &list);
    void reqAlarms(quint32 addr, quint32 count = 0);
    void reqFloats(quint32 addr, quint32 count = 0);
    void reqBitStrings(quint32 addr, quint32 count = 0);
    void setToQueue(CommandStruct &&cmd);

public slots:
    void responseHandle(const Interface::DeviceResponse &response);
    void setState(const Interface::State state) noexcept;
    void setInterfaceType(const Interface::IfaceType ifaceType) noexcept;

signals:
    void stateChanged(const Interface::State state);
    void silentReconnectMode();
    void protocolSettingsUpdated(const ProtocolDescription &desc);
    void queueSizeChanged(const quint64 size);
    void executorWakeUp();

    // Response signals
    // clazy:excludeall=overloaded-signal
    void response(const QByteArray &resp);
    void response(const DataTypes::GeneralResponseStruct &resp);
    void response(const DataTypes::BitStringStruct &resp);
    void response(const DataTypes::FloatStruct &resp);
    void response(const DataTypes::SinglePointWithTimeStruct &resp);
    void response(const DataTypes::FloatWithTimeStruct &resp);
    void response(const DataTypes::BlockStruct &resp);
#ifdef Q_OS_LINUX
    void response(const timespec &resp);
#endif
    void response(const QList<S2::DataItem> &resp);
    void response(const S2::FileStruct &resp);
    void response(const S2::S2BFile &resp);
    void response(const S2::OscInfo &resp);
    void response(const S2::SwitchJourInfo &resp);

public:
    /// \brief Функция для присоединения receiver и его слота к текущему соединению для получения данных.
    /// \details Qt::QueuedConnection используется по умолчанию, чтобы
    /// слот вызывался в контексте потока, в котором находится объект receiver.
    /// \see https://doc.qt.io/qt-5/qt.html#ConnectionType-enum
    template <typename Class, typename Slot, std::enable_if_t<std::is_member_function_pointer_v<Slot>, bool> = true> //
    inline MetaConnection connection(Class *receiver, Slot slot, Qt::ConnectionType type = Qt::QueuedConnection)
    {
        if constexpr (slot_checks<Class, Slot, DeviceResponse>)
        {
            using slot_type = typename slot_trait<Slot>::arg_type;
            return QObject::connect(this, qOverload<const slot_type &>(&AsyncConnection::response), //
                receiver, slot, type);
        }
        else
        {
            static_assert(is_qobject<Class>, "'receiver' isn't inherits QObject type");
            static_assert(is_void_ret_type<Slot>, "'slot' must have 'void' return type");
            static_assert(is_same_class<Class, Slot>, "'slot' isn't member function for 'receiver' type");
            static_assert(is_arg_variant_type<Slot, DeviceResponse>, "Argument of 'slot' isn't type of DeviceResponse");
            static_assert(is_arg_cref<Slot>, "Argument of 'slot' isn't constant reference");
        }
    }

    /// \brief Функция для присоединения receiver и ассоциированной с ним
    /// лямбда-функцией к текущему соединению для получения данных.
    /// \details Qt::QueuedConnection используется по умолчанию, чтобы
    /// лямбда-функция вызывалась в контексте потока, в котором находится объект receiver.
    /// \see https://qthub.com/static/doc/qt5/qtcore/qt.html#ConnectionType-enum
    template <typename Class, typename L, std::enable_if_t<lambda_checks<Class, L, DeviceResponse>, bool> = true> //
    inline MetaConnection connection(Class *receiver, L lambda, Qt::ConnectionType type = Qt::QueuedConnection)
    {
        using l_type = typename lambda_trait<L, DeviceResponse>::arg_type;
        return QObject::connect(this, qOverload<const l_type &>(&AsyncConnection::response), receiver, lambda, type);
    }
};

} // namespace Interface

Q_DECLARE_METATYPE(Interface::State)
Q_DECLARE_METATYPE(Interface::CommandStruct)
