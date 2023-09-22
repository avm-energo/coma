#ifndef BASECONNECTION_H
#define BASECONNECTION_H

#include "../../module/modulesettings.h"

#include <QTimer>
#include <gen/error.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>
#include <interfaces/types/common_types.h>
#include <interfaces/types/interfacesettings.h>
#include <interfaces/utils/request_queue.h>
#include <interfaces/utils/slot_trait.h>
#include <interfaces/utils/typesproxy.h>

enum INTERVAL
{
    RECONNECT = 3000,
    WAIT = 15000
};

class BasePort;       // Forward declaration
struct ConnectStruct; // Forward declaration

namespace Interface
{

namespace Regs
{
    constexpr quint16 bsiExtStartReg = 40;
    constexpr quint16 timeReg = 4600;
    constexpr quint16 bsiStartReg = 1;
    constexpr quint16 bsiCountRegs = 15;
}

class BaseConnection : public QObject
{
    Q_OBJECT

    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)

protected:
    using FileFormat = DataTypes::FileFormat;
    RequestQueue m_queue;

public:
    /// BaseInterface has its own memory manager
    /// because it can be created and deleted
    /// multiple times in runtime
    using InterfacePointer = UniquePointer<BaseConnection>;

    // protocol settings
    std::unique_ptr<ProtocolDescription> m_settings;

    explicit BaseConnection(QObject *parent = nullptr);
    ~BaseConnection() {};

    /// Pointer to current interface
    static BaseConnection *iface()
    {
        return s_connection.get();
    }

    /// Creator for interface
    static void setIface(InterfacePointer iface)
    {
        s_connection = std::move(iface);
    }

    virtual bool start(const ConnectStruct &) = 0;
    virtual bool supportBSIExt();

    // helper methods
    bool isValidRegs(const quint32 sigAdr, const quint32 sigCount, const quint32 command = 0);
    ProtocolDescription *settings();
    State state();
    void setState(const State &state);
    void close();

    // commands to send
    void reqStartup(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqBSI();
    void reqBSIExt();
    void reqFile(quint32 id, FileFormat format = FileFormat::Binary, quint32 expectedSize = 0);
    void writeFile(quint32 id, const QByteArray &ba);
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
    void connected();
    void disconnected();
    void reconnect();
    void nativeEvent(void *const message);
    void stateChanged(Interface::State m_state);
    void wakeUpParser() const;

    // Response signals
    // clazy:excludeall=overloaded-signal
    void response(const QByteArray &resp);
    void response(const DataTypes::GeneralResponseStruct &resp);
    void response(const DataTypes::BitStringStruct &resp);
    void response(const DataTypes::FloatStruct &resp);
    void response(const DataTypes::SinglePointWithTimeStruct &resp);
    void response(const DataTypes::FloatWithTimeStruct &resp);
    void response(const DataTypes::BlockStruct &resp);
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
    State m_state;
    QMutex m_stateMutex;
    UniquePointer<DataTypesProxy> proxyBS, proxyGRS, proxyFS, proxyDRL, proxyBStr;
#ifdef Q_OS_LINUX
    UniquePointer<DataTypesProxy> proxyTS;
#endif
    void ProxyInit();

protected:
    BasePort *ifacePort;

public slots:
    void responseHandle(const Interface::DeviceResponse &response);

private slots:
    void resultReady(const QVariant &msg);
    void aboba(const DataTypes::BlockStruct &msg);
    void responseReceived(const QVariant &msg);
    void fileReceived(const QVariant &msg);
    void timeout();

public:
    template <typename Class, typename Slot, std::enable_if_t<slot_checks<Class, Slot, DeviceResponse>, bool> = true> //
    inline QMetaObject::Connection connection(Class *object, Slot slot)
    {
        using slot_type = typename slot_trait<Slot>::arg_type;
        return QObject::connect(this, qOverload<const slot_type &>(&BaseConnection::response), object, slot);
    }
};

}

Q_DECLARE_METATYPE(Interface::State)
Q_DECLARE_METATYPE(Interface::CommandStruct)

#endif // BASECONNECTION_H
