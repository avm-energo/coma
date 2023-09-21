#pragma once

#include <QMutex>
#include <QVariant>
#include <gen/error.h>
#include <gen/singleton.h>
#include <queue>

class DataManager : public QObject, public Singleton<DataManager>
{
    Q_OBJECT
public:
    explicit DataManager(token, QObject *parent = nullptr) : QObject(parent)
    {
    }

    template <typename T> void addSignalToOutList(T &signal)
    {
        QVariant data;
        data.setValue(signal);
        const auto hash = std::hash<std::string> {}(typeid(T).name());
        emit DataReceived(hash, data);
    }

    template <typename T> void addToInQueue(T data)
    {
        QVariant var;
        var.setValue(data);
        QMutexLocker locker(&s_inQueueMutex);
        s_inputQueue.push(var);
    }

    template <typename T> Error::Msg deQueue(T &cmd)
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

    const size_t queueSize() const
    {
        return s_inputQueue.size();
    }

    void clearQueue()
    {
        decltype(s_inputQueue) empty;
        std::swap(s_inputQueue, empty);
    }

private:
    std::queue<QVariant> s_inputQueue;
    QMutex s_inQueueMutex;

signals:
    void DataReceived(const std::size_t &, const QVariant &);
};
