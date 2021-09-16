#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "datatypes.h"
#include "error.h"
#include "singleton.h"

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QVariant>
#include <queue>
#ifdef __linux__
#include <time.h>
#endif
#define INQUEUEMAXSIZE 100

template <class> inline constexpr bool always_false_v = false;

class DataManager : public QObject, public Singleton<DataManager>
{
    Q_OBJECT
public:
    using RegisterType = std::variant<DataTypes::BitStringStruct, //
        DataTypes::SinglePointWithTimeStruct,                     //
        DataTypes::FloatStruct>;
    explicit DataManager(token, QObject *parent = nullptr);

    //    static Error::Msg getSignals(quint32 firstSignalAdr, quint32 lastSignalAdr, DataTypes::SignalTypes type,
    //        QList<DataTypes::SignalsStruct> &outlist);
    //    static Error::Msg getFile(quint32 filenum, QByteArray &outba);
    //    static Error::Msg getConfig(quint32 firstID, quint32 lastID, QList<DataTypes::ConfParameterStruct> &outlist);
    //    static Error::Msg getResponse(DataTypes::GeneralResponseTypes type, DataTypes::GeneralResponseStruct
    //    &response); static void setConfig(S2ConfigType *s2config);
    //    static void reqStartup();
    void checkTypeAndSendSignals(DataTypes::SignalsStruct &str);
    template <typename T> static void addSignalToOutList(DataTypes::SignalTypes type, T signal)
    {
        DataTypes::SignalsStruct str;
        str.type = type;
        str.data.setValue(signal);
        GetInstance().checkTypeAndSendSignals(str);
        //        s_outListMutex.lock();
        //        s_outputList.append(str);
        //        s_outListMutex.unlock();
    }
    template <typename T> static void addToInQueue(T data)
    {
        QVariant var;
        var.setValue(data);
        QMutexLocker locker(&s_inQueueMutex);
        s_inputQueue.push(var);
    }
    template <typename T> static Error::Msg deQueue(T &cmd)
    {
        QMutexLocker locker(&s_inQueueMutex);
        if (!s_inputQueue.empty())
        {
            QVariant inp = s_inputQueue.front();
            if (inp.canConvert<T>())
            {
                s_inputQueue.pop();
                cmd = qvariant_cast<T>(inp);
                return Error::Msg::NoError;
            }
        }
        return Error::Msg::ResEmpty;
    }
    static size_t queueSize()
    {
        return s_inputQueue.size();
    }
    static void clearQueue()
    {
        decltype(s_inputQueue) empty;
        std::swap(s_inputQueue, empty);
        GetInstance().m_registers.clear();
    }
    bool containsRegister(quint32 addr) const
    {
        QMutexLocker locker(&s_inQueueMutex);
        return m_registers.contains(addr);
    }
    template <typename T> bool containsRegister(quint32 addr) const
    {
        if (containsRegister(addr))
        {
            QMutexLocker locker(&s_inQueueMutex);
            return std::holds_alternative<T>(m_registers.value(addr));
        }
        return false;
    }

    template <typename T> T getRegister(quint32 addr) const
    {
        Q_ASSERT(containsRegister<T>(addr));
        QMutexLocker locker(&s_inQueueMutex);
        return std::get<T>(m_registers.value(addr));
    }

private:
    static std::queue<QVariant> s_inputQueue;
    //    static QList<DataTypes::SignalsStruct> s_outputList;
    //    static QMutex s_outListMutex;
    static QMutex s_inQueueMutex;

    QMap<quint32, RegisterType> m_registers;

    template <typename T> void insertRegister(quint32 addr, T value)
    {
        QMutexLocker locker(&s_inQueueMutex);
        if constexpr ((std::is_same_v<T, DataTypes::BitStringStruct>)    //
            || (std::is_same_v<T, DataTypes::SinglePointWithTimeStruct>) //
            || (std::is_same_v<T, DataTypes::FloatStruct>))              //
        {
            m_registers.insert(addr, value);
        }
        else
            static_assert(always_false_v<T>, "unsupported type!");
    }

signals:
    void dataReceived(const DataTypes::SignalsStruct &);
    void bitStringReceived(const DataTypes::BitStringStruct &);
    void singlePointReceived(const DataTypes::SinglePointWithTimeStruct &);
    void floatReceived(const DataTypes::FloatStruct &);
    void fileReceived(const DataTypes::FileStruct &);
    void dataRecVListReceived(const QList<DataTypes::DataRecV> &);
    void responseReceived(const DataTypes::GeneralResponseStruct &);
    void oscInfoReceived(const S2DataTypes::OscInfo &);
    void swjInfoReceived(const S2DataTypes::SwitchJourInfo &);
    void blockReceived(const DataTypes::BlockStruct &);
#ifdef __linux__
    void timeReceived(const timespec &);
#endif
};

#endif // DATAMANAGER_H
