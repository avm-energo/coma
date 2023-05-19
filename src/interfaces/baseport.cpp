#include "baseport.h"

#include <QCoreApplication>
#include <QElapsedTimer>

BasePort::BasePort(const QString &logFilename, QObject *parent) : QObject(parent), Log(new LogClass(this))
{
    QString filename(logFilename);
    filename.append(".").append(::logExt);
    Log->Init(filename);
    Log->WriteRaw(::logStart);
    // QObject::connect(this, &BasePort::tryToReconnect, this, &BasePort::reconnect);
}

void BasePort::setState(Interface::State state)
{
    m_state = state;
    emit stateChanged(m_state);
}

bool BasePort::reconnect()
{
    Log->WriteRaw("!!! Restart connection !!!\n");
    disconnect();
    QElapsedTimer *tmr = new QElapsedTimer;
    tmr->start();
    while (tmr->elapsed() < RECONNECTINTERVAL)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    delete tmr;
    return connect();
}
