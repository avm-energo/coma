#include "baseport.h"

#include <QCoreApplication>
#include <QElapsedTimer>

BasePort::BasePort(const QString &logFilename, QObject *parent)
    : QObject(parent), m_state(Interface::State::Connect), m_log(new LogClass(this)), m_timeoutTimer(new QTimer(this))
{
    QString filename(logFilename);
    filename.append(".").append(::logExt);
    m_log->Init(filename);
    m_log->WriteRaw(::logStart);
}

void BasePort::setState(Interface::State state)
{
    QMutexLocker locker(&m_stateGuard);
    m_state = state;
    emit stateChanged(m_state);
}

Interface::State BasePort::getState()
{
    QMutexLocker locker(&m_stateGuard);
    return m_state;
}

bool BasePort::reconnect()
{
    setState(Interface::State::Reconnect);
    m_log->WriteRaw("!!! Restart connection !!!\n");
    disconnect();
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < RECONNECTINTERVAL)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    return connect();
}

void BasePort::poll(bool ok)
{
    Interface::State state;
    do
    {
        state = getState();
        if (state == Interface::State::Run)
        {
            bool status = true;
            auto data = readData();
            if (!data.isEmpty())
                emit dataReceived(data);
        }
        else
        {
            StdFunc::Wait(500);
            state = getState();
        }
    } while (state != Interface::State::Disconnect);
    // finish();
}
