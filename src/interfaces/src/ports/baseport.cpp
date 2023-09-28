#include "interfaces/ports/baseport.h"

#include <QCoreApplication>
#include <QElapsedTimer>

BasePort::BasePort(const QString &logFilename, QObject *parent) : QObject(parent), m_state(Interface::State::Connect)
{
    m_log.init(logFilename + "." + ::logExt);
    m_log.writeRaw(::logStart);
}

void BasePort::setState(Interface::State state)
{
    m_state.store(state);
    emit stateChanged(state);
}

Interface::State BasePort::getState()
{
    return m_state.load();
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
    m_log.writeRaw(tmpba);
}

void BasePort::writeLog(const Error::Msg msg, Interface::Direction dir)
{
    writeLog(QVariant::fromValue(msg).toByteArray(), dir);
}

bool BasePort::reconnect()
{
    setState(Interface::State::Reconnect);
    m_log.writeRaw("!!! Restart connection !!!\n");
    disconnect();
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < RECONNECTINTERVAL)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    emit clearQueries();
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
    disconnect();
    m_log.info(QString(metaObject()->className()) + " is finished\n");
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
    emit clearQueries();
}
