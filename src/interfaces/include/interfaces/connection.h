#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTimer>
#include <gen/error.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>
#include <interfaces/types/common_types.h>
#include <interfaces/utils/request_queue.h>
#include <interfaces/utils/slot_trait.h>

namespace Interface
{

namespace Regs
{
    constexpr quint16 bsiStartReg = 1;
    constexpr quint16 bsiCountRegs = 15;
    constexpr quint16 bsiExtStartReg = 40;
    constexpr quint16 timeReg = 4600;
}

class Connection : public QObject
{
    Q_OBJECT

protected:
    using FileFormat = DataTypes::FileFormat;
    RequestQueue m_queue;

public:
    /// BaseInterface has its own memory manager
    /// because it can be created and deleted
    /// multiple times in runtime
    using InterfacePointer = UniquePointer<Connection>;

    explicit Connection(QObject *parent = nullptr);

    /// \brief Getter for the current connection.
    static Connection *iface() noexcept;
    /// \brief Updating the current connection.
    static void update(InterfacePointer iface) noexcept;

    RequestQueue &getQueue() noexcept;
    bool supportBSIExt();

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
    void reqAlarms(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqFloats(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0);
    void setToQueue(CommandStruct &&cmd);

    // ===============================================================================
    // =============================== SYNC METHODS ==================================
    // ===============================================================================

    Error::Msg reqBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeFileSync(S2::FilesEnum filenum, const QByteArray &ba);
    Error::Msg readS2FileSync(S2::FilesEnum filenum);
    Error::Msg readFileSync(S2::FilesEnum filenum, QByteArray &ba);
    Error::Msg reqTimeSync(void *block, quint32 blocksize);

signals:
    void stateChanged(Interface::State m_state);
    void silentReconnectMode();

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

private:
    static InterfacePointer s_connection;
    bool m_busy, m_timeout;
    QByteArray m_byteArrayResult;
    bool m_responseResult;
    QTimer *m_timeoutTimer;

public slots:
    void responseHandle(const Interface::DeviceResponse &response);

private slots:
    void resultReady(const DataTypes::BlockStruct &result);
    void responseReceived(const DataTypes::GeneralResponseStruct &response);
    void fileReceived(const S2::FileStruct &file);
    void timeout();

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
            return QObject::connect(this, qOverload<const slot_type &>(&Connection::response), //
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
        return QObject::connect(this, qOverload<const l_type &>(&Connection::response), receiver, lambda, type);
    }
};
}

Q_DECLARE_METATYPE(Interface::State)
Q_DECLARE_METATYPE(Interface::CommandStruct)

#endif // CONNECTION_H
