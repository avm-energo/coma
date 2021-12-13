#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H
#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/logclass.h"
#include "../gen/settings.h"
#include "../gen/stdfunc.h"
#include "interfacesettings.h"

#include <QTimer>
#include <memory>
#include <typeinfo>

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
    /// BaseInterface has our own memory manager
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

public:
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
    virtual void reqBSIExt() = 0;
    virtual void reqFile(quint32, FileFormat format = FileFormat::Binary) = 0;
    void reqFile(quint32 id, FileFormat format, quint32 expectedSize);
    virtual void writeFile(quint32, const QByteArray &) = 0;
    void writeS2File(DataTypes::FilesEnum number, S2DataTypes::S2ConfigType *file);
    // void writeConfigFile();
    virtual void reqTime() = 0;
    virtual void writeTime(quint32) = 0;
    virtual void writeCommand(Queries::Commands, QVariant = 0) = 0;
    virtual void writeCommand(Queries::Commands cmd, const QVariantList &list)
    {
        // for each signal in list form the command and set it into the input queue
        for (const auto &item : list)
            writeCommand(cmd, item);
    }

    void reqAlarms(quint32 sigAdr = 0, quint32 sigCount = 0);
    virtual void reqFloats(quint32 sigAdr = 0, quint32 sigCount = 0) = 0;
    virtual void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0) = 0;
    /// Прямая запись данных
    virtual void writeRaw(const QByteArray &)
    {
    }
    //    bool isWorking()
    //    {
    //        return m_working;
    //    }

    // Bac & Bda blocks only supported for now
    Error::Msg reqBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeConfFileSync(const QList<DataTypes::DataRecV> &config);
    Error::Msg writeFileSync(int filenum, QByteArray &ba);
    Error::Msg writeS2FileSync(DataTypes::FilesEnum number, S2DataTypes::S2ConfigType *file);
    Error::Msg readS2FileSync(quint32 filenum);
    Error::Msg readFileSync(quint32 filenum, QByteArray &ba);
    Error::Msg reqTimeSync(void *block, quint32 blocksize);
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

    template <class Group> InterfaceSettings parseSettings(QDomElement domElement) const
    {
        using Register = typename InterfaceInfo<Group>::Register;
#ifdef XML_DEBUG
        qDebug() << domElement.text();
        qDebug() << "TagName: " << domElement.tagName();
#endif
        const auto &nodes = domElement.childNodes();
        // NOTE Temporary commented
        // Q_ASSERT(!nodes.isEmpty());
        int i = 0;
        InterfaceInfo<Group> settings;

        while (i != nodes.count())
        {
            const auto &domElement = nodes.item(i++).toElement();
            if (domElement.tagName().contains("group", Qt::CaseInsensitive))
            {
                Group group(domElement);
                settings.addGroup(group);
            }
            else if (domElement.tagName().contains("register", Qt::CaseInsensitive))
            {
                Register reg(domElement);
                settings.addReg(reg);
            }
#ifdef XML_DEBUG
            qDebug() << group.attribute("id", "") << group.text();
#endif
        }
#ifdef XML_DEBUG
        qDebug() << settings.groups().count();
#endif
        return InterfaceSettings { QVariant::fromValue(settings) };
    }
    virtual InterfaceSettings parseSettings(QDomElement domElement) const = 0;

    bool virtual supportBSIExt();

signals:
    void reconnect();
    void finish();
    void nativeEvent(void *const message);
    void stateChanged(BaseInterface::State m_state);

protected:
private:
    bool m_busy, m_timeout;
    QByteArray m_byteArrayResult;
    bool m_responseResult;

    QTimer *timeoutTimer;
    static InterfacePointer m_iface;
    State m_state;
    InterfaceSettings m_settings;

    QMutex _stateMutex;

private slots:
    void resultReady(const DataTypes::BlockStruct &result);
    void responseReceived(const DataTypes::GeneralResponseStruct &response);
    void fileReceived(const DataTypes::FileStruct &file);
    void timeout();
};

Q_DECLARE_METATYPE(BaseInterface::State)

#endif // BASEINTERFACE_H
