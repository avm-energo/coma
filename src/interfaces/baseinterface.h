#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H

#include "../module/modulesettings.h"
#include "../s2/configv.h"
#include "../s2/datarecv.h"

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

class BaseInterface : public QObject
{
    Q_OBJECT

    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
protected:
    using FileFormat = Queries::FileFormat;

public:
    /// BaseInterface has its own memory manager
    /// because it can be created and deleted
    /// multiple times in runtime
    using InterfacePointer = UniquePointer<BaseInterface>;

    enum State
    {
        Run,
        Stop,
        Wait,
        None
    };

    enum BI_Commands
    {
        C_ReqStartup,
        C_ReqBSI,
        C_ReqBSIExt,
        C_ReqAlarms,
        C_ReqFile,
        C_WriteFile,
        C_ReqTime,
        C_WriteTime,
        C_WriteCommand,
        C_ReqFloats,
        C_ReqBitStrings
    };

    struct BI_CommandStruct
    {
        BI_Commands command;
        QVariant arg1; // reqFile, writeFile: id, reqStartup: sigAddr, WriteTime: time, WriteCommand: command
        QVariant arg2; // reqFile: format, reqStartup: sigCount, WriteFile: &bytearray, WriteCommand: value
    };

    UniquePointer<LogClass> Log;

    explicit BaseInterface(QObject *parent = nullptr);
    ~BaseInterface();
    /// Pointer to current interface
    static BaseInterface *iface();
    /// Creator for interface
    static void setIface(InterfacePointer iface);

    virtual bool start(const ConnectStruct &) = 0;
    virtual void pause();
    virtual void resume();
    virtual void stop();

    // commands to send
    void reqStartup(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqBSI();
    void reqBSIExt();
    void reqFile(quint32 id, FileFormat format = FileFormat::Binary, quint32 expectedSize = 0);
    void writeFile(quint32 id, const QByteArray &ba);
    void writeS2File(DataTypes::FilesEnum number, S2DataTypes::S2ConfigType *file);
    void reqTime();
    void writeTime(quint32 time);
    void writeCommand(Queries::Commands cmd, QVariant value = 0);
    void writeCommand(Queries::Commands cmd, const QVariantList &list)
    {
        // for each signal in list form the command and set it into the input queue
        for (const auto &item : list)
            writeCommand(cmd, item);
    }
    void reqAlarms(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqFloats(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0);

    // Bac & Bda blocks only supported for now
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

    ModuleTypes::InterfaceSettings settings() const;

    template <class T> T settings() const
    {
        Q_ASSERT(m_settings.settings.canConvert<T>());
        return m_settings.settings.value<T>();
    }

    void setSettings(const InterfaceSettings &settings);
    template <class T> void setSettings(const T &settings)
    {
        m_settings.settings = settings;
    }

    State state();
    void setState(const State &state);
    bool virtual supportBSIExt();

signals:
    void reconnect();
    void finish();
    void nativeEvent(void *const message);
    void stateChanged(BaseInterface::State m_state);

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
    void timeout();
};

Q_DECLARE_METATYPE(BaseInterface::State)
Q_DECLARE_METATYPE(BaseInterface::BI_CommandStruct)

#endif // BASEINTERFACE_H
