#ifndef BASEINTERFACETHREAD_H
#define BASEINTERFACETHREAD_H

#include "../s2/filestruct.h"
#include "baseinterface.h"

#include <QObject>
#include <QWaitCondition>
#include <gen/datatypes.h>

class BaseInterfaceThread : public QObject
{
    Q_OBJECT
public:
    explicit BaseInterfaceThread(QObject *parent = nullptr);

    virtual void clear();
    void wakeUp();

    void FilePostpone(QByteArray &ba, DataTypes::FilesEnum addr, Queries::FileFormat format);
    void checkQueue();
    virtual void parseRequest(const BaseInterface::BI_CommandStruct &cmdStr) = 0;

    bool m_isCommandRequested = false;
    quint64 m_progress = 0;
    BaseInterface::BI_CommandStruct m_currentCommand;
    QMutex _mutex;

    QWaitCondition _waiter;

protected:
    using FileFormat = Queries::FileFormat;

signals:
};

#endif // BASEINTERFACETHREAD_H
