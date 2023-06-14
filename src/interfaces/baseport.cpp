#include "baseport.h"

#include <QCoreApplication>
#include <QElapsedTimer>

BasePort::BasePort(const QString &logFilename, QObject *parent)
    : QObject(parent), m_state(Interface::State::Connect), m_log(new LogClass(this))
{
    m_log->Init(logFilename + "." + ::logExt);
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

void BasePort::writeLog(const QByteArray &ba, Interface::Direction dir)
{
    QByteArray tmpba = QByteArray(metaObject()->className());
    switch (dir)
    {
    case Interface::FromDevice:
        tmpba.append(": -> ");
        break;
    case Interface::ToDevice:
        tmpba.append(": <- ");
        break;
    default:
        tmpba.append(":  ");
        break;
    }
    tmpba.append(ba).append("\n");
    m_log->WriteRaw(tmpba);
}

void BasePort::writeLog(const Error::Msg msg, Interface::Direction dir)
{
    writeLog(QVariant::fromValue(msg).toByteArray(), dir);
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

void BasePort::poll()
{
    Interface::State state;
    do
    {
        state = getState();
        if (state == Interface::State::Disconnect)
            break;
        else if (state == Interface::State::Run)
        {
            bool status = true;
            auto data = read(&status); // read data
            if (!data.isEmpty() && status)
            {
                writeLog(data.toHex(), Interface::Direction::FromDevice);
                emit dataReceived(data);
            }
        }
        else
        {
            StdFunc::Wait(200);
            state = getState();
        }
    } while (state != Interface::State::Disconnect);

    // Finish thread
    QMutexLocker locker(&m_dataGuard);
    disconnect();
    m_log->info(QString(metaObject()->className()) + " is finished\n");
    emit finished();
    QCoreApplication::processEvents();
}

void BasePort::writeDataSync(const QByteArray &ba)
{
    auto status = Error::Msg::NoError;
    if (getState() == Interface::State::Run) // port is running
    {
        if (!ba.isEmpty())
        {
            auto success = write(ba); // write data
            if (success)
                writeLog(ba.toHex(), Interface::Direction::ToDevice);
            else
                status = Error::Msg::WriteError;
        }
        else
            status = Error::Msg::NullDataError;
    }
    if (status != Error::Msg::NoError)
    {
        writeLog(status);
        qCritical() << status;
    }
}

void BasePort::closeConnection()
{
    setState(Interface::State::Disconnect);
}
