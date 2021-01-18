#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H
#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/logclass.h"
#include "interfacesettings.h"

#include <QTimer>
#include <memory>
#include <typeinfo>

struct ConnectStruct;
struct InterfaceSettings;
class BaseInterface : public QObject
{
    Q_OBJECT

private:
    struct deleteLaterDeletor
    {
        void operator()(QObject *object) const
        {
            if (object)
            {
                object->deleteLater();
            }
        }
    };

public:
    /// BaseInterface has our own memory manager
    /// because it can be created and deleted
    /// multiple times in runtime
    using InterfacePointer = std::unique_ptr<BaseInterface, deleteLaterDeletor>;
    // using TimerPointer=std::unique_ptr<BaseInterface, deleteLaterDeletor>
    enum State
    {
        Run,
        Stop,
        Wait,
        None
    };

public:
    bool m_working;
    LogClass *Log;

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
    virtual void reqStartup(quint32 sigAdr = 0, quint32 sigCount = 0) = 0;
    virtual void reqBSI() = 0;
    virtual void reqFile(quint32, bool isConfigFile = false) = 0;
    virtual void writeFile(quint32, const QByteArray &) = 0;
    void writeS2File(DataTypes::FilesEnum, S2DataTypes::S2ConfigType *);
    void writeConfigFile();
    virtual void reqTime() = 0;
    virtual void writeTime(quint32) = 0;
    virtual void writeCommand(Queries::Commands, QVariant = 0) = 0;
    void writeCommand(Queries::Commands cmd, const QList<QVariant> &list)
    {
        // for each signal in list form the command and set it into the input queue
        for (const auto &item : list)
            writeCommand(cmd, item);
    }

    void reqAlarms(quint32 sigAdr = 0, quint32 sigCount = 0);
    virtual void reqFloats(quint32 sigAdr = 0, quint32 sigCount = 0) = 0;
    //    virtual void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0) = 0;
    /// Прямая запись данных
    virtual void writeRaw(const QByteArray &)
    {
    }
    bool isWorking()
    {
        return m_working;
    }

    // Bac & Bda blocks only supported for now
    Error::Msg reqBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeConfFileSync();
    Error::Msg writeFileSync(int filenum, QByteArray &ba);
    Error::Msg readS2FileSync(quint32 filenum);
    Error::Msg readFileSync(quint32 filenum, QByteArray &ba);

    InterfaceSettings settings() const;
    template <class T> T settings() const
    {
        Q_ASSERT(m_settings.settings.canConvert<T>());
        // qDebug() << m_settings.settings.type().name() << "<->" << typeid(T).name();
        //  Q_ASSERT(m_settings.settings.type() == typeid(T));
        //    Q_ASSERT(std::holds_alternative<T>(m_settings.settings));
        //   return std::get<T>(m_settings.settings);
        return m_settings.settings.value<T>();
    }
    void setSettings(const InterfaceSettings &settings);
    template <class T> void setSettings(const T &settings)
    {
        m_settings.settings = settings;
    }

    State state();
    void setState(const State &state);

    virtual InterfaceSettings parseSettings(QDomElement domElement) const = 0;

signals:
    void reconnect();
    void finish();

    void stateChanged(State m_state);

private:
    bool m_busy, m_timeout;
    QByteArray m_byteArrayResult;
    bool m_responseResult;
    static InterfacePointer m_iface;
    QTimer *timeoutTimer;
    State m_state;
    InterfaceSettings m_settings;

    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)

    QMutex _stateMutex;

private slots:
    void resultReady(const DataTypes::BlockStruct &result);
    void responseReceived(const DataTypes::GeneralResponseStruct &response);
    void confParameterBlockReceived(const DataTypes::ConfParametersListStruct &cfpl);
    void fileReceived(const DataTypes::FileStruct &file);
    void timeout();
};

#endif // BASEINTERFACE_H
