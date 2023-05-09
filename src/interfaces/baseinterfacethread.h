#ifndef BASEINTERFACETHREAD_H
#define BASEINTERFACETHREAD_H

#include "../s2/filestruct.h"
#include "baseinterface.h"

#include <QObject>
#include <QWaitCondition>
#include <gen/datatypes.h>

namespace Interface
{

class BaseInterfaceThread : public QObject
{
    Q_OBJECT
public:
    explicit BaseInterfaceThread(QObject *parent = nullptr);

    virtual void clear();
    void wakeUp();

    quint16 blockByReg(const quint32 regAddr)
    {
        return BaseInterface::iface()->settings().dictionary().value(regAddr).dataType.value<quint16>();
    }

    void FilePostpone(QByteArray &ba, DataTypes::FilesEnum addr, DataTypes::FileFormat format);
    void checkQueue();
    virtual void parseRequest(const CommandStruct &cmdStr) = 0;
    virtual void parseResponse() = 0;

    bool m_isCommandRequested = false;
    quint64 m_progress = 0;
    CommandStruct m_currentCommand;
    QMutex _mutex;
    QByteArray m_readData;

    QWaitCondition _waiter;

protected:
    using FileFormat = DataTypes::FileFormat;

signals:

public slots:
    void run();
    virtual void processReadBytes(QByteArray &ba) = 0;
};

}

#endif // BASEINTERFACETHREAD_H
