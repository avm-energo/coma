#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H

#include "../module/modulesettings.h"
#include "../s2/configv.h"
#include "../s2/datarecv.h"
#include "../s2/filestruct.h"
#include "interfacesettings.h"

#include <QTimer>
#include <gen/datamanager/typesproxy.h>
#include <gen/datatypes.h>
#include <gen/error.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>

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

enum State
{
    Connect,
    Reconnect,
    Disconnect,
    Run
};

enum Direction
{
    NoDirection,
    ToDevice,
    FromDevice
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
    C_ReqProgress,
    C_SetNewConfiguration,
    C_StartFirmwareUpgrade,
    C_EraseJournals,

    C_StartWorkingChannel,
    C_SetStartupValues,
    C_SetStartupPhaseA,
    C_SetStartupPhaseB,
    C_SetStartupPhaseC,
    C_SetStartupUnbounced,
    C_SetTransOff,
    C_ClearStartupValues,
    C_ClearStartupUnbounced,
    C_ClearStartupError,

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

enum CommandRegisters
{
    StartWorkingChannel = 803,  ///< Старт рабочего канала
    SetStartupValues = 900,     ///< Задать начальные значения по всем фазам
    SetStartupPhaseA = 901,     ///< Задать начальные значения по фазе A
    SetStartupPhaseB = 902,     ///< Задать начальные значения по фазе B
    SetStartupPhaseC = 903,     ///< Задать начальные значения по фазе C
    SetStartupUnbounced = 904,  ///< Задать начальные значения по току небаланса
    ClearStartupValues = 905,   ///< Сбросить начальные значения по всем фазам
    ClearStartupSetError = 906, ///< Сбросить ошибку задания начальных значений
    SetTransOff = 907,          ///< Послать команду "Трансфоратор отключён"
    ClearStartupUnbounced = 908 ///< Сбросить начальное значение тока небаланса
};

enum TechBlocks
{
    T_Oscillogram = 0x01,
    T_GeneralEvent = 0x02,
    T_TechEvent = 0x03,
    T_SwitchJournal = 0x04,
    T_WorkArchive = 0x05
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

    // protocol settings
    std::unique_ptr<ProtocolDescription> m_settings;

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
    void writeS2File(DataTypes::FilesEnum number, const S2DataTypes::S2ConfigType &file);
    void reqTime();
    void writeTime(quint32 time);
    void writeCommand(Commands cmd, QVariant value = 0);
    void writeCommand(Commands cmd, const QVariantList &list);
    void reqAlarms(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqFloats(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0);
    void setToQueue(CommandStruct &cmd);

    // ===============================================================================
    // =============================== SYNC METHODS ==================================
    // ===============================================================================

    Error::Msg reqBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeConfFileSync(const QList<DataTypes::DataRecV> &config);
    Error::Msg pushAndWriteConfFileSync(ConfigV *config, const QList<DataTypes::DataRecV> recordList);
    Error::Msg popAndWriteConfFileSync(ConfigV *config);
    Error::Msg writeFileSync(int filenum, QByteArray &ba);
    Error::Msg writeS2FileSync(DataTypes::FilesEnum number, const S2DataTypes::S2ConfigType &file);
    Error::Msg readS2FileSync(quint32 filenum);
    Error::Msg readFileSync(quint32 filenum, QByteArray &ba);
    Error::Msg reqTimeSync(void *block, quint32 blocksize);

signals:
    void connected();
    void disconnected();
    void reconnect();
    void nativeEvent(void *const message);
    void stateChanged(State m_state);
    void wakeUpParser() const;

private:
    bool m_busy, m_timeout;
    QByteArray m_byteArrayResult;
    bool m_responseResult;
    QTimer *m_timeoutTimer;
    static InterfacePointer m_iface;
    State m_state;
    QMutex _stateMutex;
    UniquePointer<DataTypesProxy> proxyBS, proxyGRS, proxyFS, proxyDRL, proxyBStr;
#ifdef __linux__
    UniquePointer<DataTypesProxy> proxyTS;
#endif
    void ProxyInit();

protected:
    BasePort *ifacePort;

private slots:
    void resultReady(const QVariant &msg);
    void responseReceived(const QVariant &msg);
    void fileReceived(const QVariant &msg);
    void timeout();
};

}

Q_DECLARE_METATYPE(Interface::State)
Q_DECLARE_METATYPE(Interface::CommandStruct)

#endif // BASEINTERFACE_H
