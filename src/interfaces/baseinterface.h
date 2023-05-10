#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H

#include "../module/modulesettings.h"
#include "../s2/configv.h"
#include "../s2/datarecv.h"
#include "interfacesettings.h"

#include <QTimer>
#include <gen/datamanager/typesproxy.h>
#include <gen/datatypes.h>
#include <gen/error.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>
#include <memory>
#include <typeinfo>

using InterfaceSettings = ModuleTypes::InterfaceSettings;

enum INTERVAL
{
    RECONNECT = 3000,
    WAIT = 15000
};

struct ConnectStruct;

namespace Interface
{

enum State
{
    Run,
    Stop,
    Wait,
    None
};

enum Commands
{
    C_ReqStartup,
    C_ReqBSI,
    C_ReqBSIExt,
    C_ReqAlarms,
    C_ReqFile,
    C_WriteFile,
    C_ReqTime,
    C_WriteTime,
    C_ReqFloats,
    C_ReqBitStrings,
    C_SetNewConfiguration,
    C_StartFirmwareUpgrade,
    C_StartWorkingChannel,
    C_EraseJournals,
    C_SetStartupValues,
    C_SetStartupPhaseA,
    C_SetStartupPhaseB,
    C_SetStartupPhaseC,
    C_SetStartupUnbounced,
    C_SetTransOff,
    C_ClearStartupValues,
    //    C_ClearStartupPhaseA,
    //    C_ClearStartupPhaseB,
    //    C_ClearStartupPhaseC,
    C_ClearStartupUnbounced,
    C_ClearStartupError,
    C_Command50,
    C_Test,
    C_EraseTechBlock,
    C_WriteHiddenBlock,
    C_WriteUserValues,
    C_WriteSingleCommand,
    C_ReqTuningCoef,
    C_WriteTuningCoef,
    C_WriteBlkData,
    C_ReqBlkData,
    C_ReqBlkDataA,
    C_ReqBlkDataTech,
    C_WriteBlkDataTech,
    C_Reboot,
    C_ReqOscInfo,
    C_SetMode,
    C_GetMode,
    C_WriteHardware
};

struct CommandStruct
{
    Commands command;
    QVariant arg1; // reqFile, writeFile: id, reqStartup: sigAddr, WriteTime: time, WriteCommand: command
    QVariant arg2; // reqFile: format, reqStartup: sigCount, WriteFile: &bytearray, WriteCommand: value
};

class BaseInterface : public QObject
{
    Q_OBJECT

    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
protected:
    using FileFormat = DataTypes::FileFormat;

public:
    /// BaseInterface has its own memory manager
    /// because it can be created and deleted
    /// multiple times in runtime
    using InterfacePointer = UniquePointer<BaseInterface>;

    UniquePointer<LogClass> Log;

    explicit BaseInterface(QObject *parent = nullptr);
    ~BaseInterface() {};

    /// Pointer to current interface
    static BaseInterface *iface()
    {
        return m_iface.get();
    }

    /// Creator for interface
    static void setIface(InterfacePointer iface)
    {
        m_iface = std::move(iface);
    }

    virtual bool start(const ConnectStruct &) = 0;
    virtual void pause()
    {
        setState(State::Wait);
    }
    virtual void resume()
    {
        // Only for case Wait to Run
        Q_ASSERT(state() == State::Wait);
        setState(State::Run);
    }
    virtual void stop()
    {
        Log->info("Stop()");
        setState(State::Stop);
        qInfo() << metaObject()->className() << "disconnected";
    }

    // commands to send
    void reqStartup(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqBSI();
    void reqBSIExt();
    void reqFile(quint32 id, FileFormat format = FileFormat::Binary, quint32 expectedSize = 0);
    void writeFile(quint32 id, const QByteArray &ba);
    void writeS2File(DataTypes::FilesEnum number, S2DataTypes::S2ConfigType *file);
    void reqTime();
    void writeTime(quint32 time);
    void writeCommand(Commands cmd, QVariant value = 0);
    void writeCommand(Commands cmd, const QVariantList &list);
    void reqAlarms(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqFloats(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0);

    // Synchronized requests for Bac & Bda blocks are only supported for now
    Error::Msg reqBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeConfFileSync(const QList<DataTypes::DataRecV> &config);
    Error::Msg pushAndWriteConfFileSync(ConfigV *config, const QList<DataTypes::DataRecV> recordList);
    Error::Msg popAndWriteConfFileSync(ConfigV *config);
    Error::Msg writeFileSync(int filenum, QByteArray &ba);
    Error::Msg writeS2FileSync(DataTypes::FilesEnum number, S2DataTypes::S2ConfigType *file);
    Error::Msg readS2FileSync(quint32 filenum);
    Error::Msg readFileSync(quint32 filenum, QByteArray &ba);
    Error::Msg reqTimeSync(void *block, quint32 blocksize);

    bool isValidRegs(const quint32 sigAdr, const quint32 sigCount, const quint32 command = 0)
    {
        const auto &st = settings();
        if (!st.dictionary().contains(sigAdr))
            return false;
        const auto val = st.dictionary().value(sigAdr);
        if (command != 0)
        {
            if (command != val.function)
                return false;
        }
        return val.count == sigCount;
    }

    ModuleTypes::InterfaceSettings interfaceSettings() const
    {
        return m_settings;
    }

    ProtocolDescription settings() const
    {
        return m_settings.settings;
    }

    void setInterfaceSettings(const InterfaceSettings &settings)
    {
        m_settings = settings;
    }

    void setSettings(const ProtocolDescription &settings)
    {
        m_settings.settings = settings;
    }

    State state()
    {
        QMutexLocker locker(&_stateMutex);
        return m_state;
    }
    void setState(const State &state)
    {
        QMutexLocker locker(&_stateMutex);
        m_state = state;
        emit stateChanged(m_state);
    }

    bool virtual supportBSIExt();

signals:
    void reconnect();
    void finish();
    void nativeEvent(void *const message);
    void stateChanged(State m_state);

private:
    bool m_busy, m_timeout;
    QByteArray m_byteArrayResult;
    bool m_responseResult;
    QTimer *timeoutTimer;
    static InterfacePointer m_iface;
    State m_state;
    InterfaceSettings m_settings;
    QMutex _stateMutex;
    UniquePointer<DataTypesProxy> proxyBS, proxyGRS, proxyFS, proxyDRL, proxyBStr;
#ifdef __linux__
    UniquePointer<DataTypesProxy> proxyTS;
#endif
    void ProxyInit();

private slots:
    void resultReady(const QVariant &msg);
    void responseReceived(const QVariant &msg);
    void fileReceived(const QVariant &msg);
    void timeout()
    {
        m_busy = false;
    }
};

}

Q_DECLARE_METATYPE(Interface::State)
Q_DECLARE_METATYPE(Interface::CommandStruct)

#endif // BASEINTERFACE_H
