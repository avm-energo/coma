#include "interfaces/ifaces/baseinterface.h"

#include <QCoreApplication>
#include <QElapsedTimer>

BaseInterface::BaseInterface(const QString &logFilename, QObject *parent)
    : QObject(parent), m_state(Interface::State::Connect)
{
    qRegisterMetaType<InterfaceError>();
    m_log.init(logFilename + "." + ::logExt);
    m_log.writeRaw(::logStart);
}

void BaseInterface::setState(const Interface::State state) noexcept
{
    m_state.store(state);
    emit stateChanged(state);
}

Interface::State BaseInterface::getState() const noexcept
{
    return m_state.load();
}

void BaseInterface::writeLog(const QByteArray &ba, Interface::Direction dir)
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

void BaseInterface::writeLog(const Error::Msg msg, Interface::Direction dir)
{
    writeLog(QVariant::fromValue(msg).toByteArray(), dir);
}

void BaseInterface::poll()
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
            QCoreApplication::processEvents();
            state = getState();
        }
    } while (state != Interface::State::Disconnect);

    // Finish thread
    disconnect();
    m_log.info(QString(metaObject()->className()) + " is finished\n");
    emit finished();
    QCoreApplication::processEvents();
}

void BaseInterface::writeData(const QByteArray &ba)
{
    auto status = Error::Msg::NoError;
    if (getState() == Interface::State::Run) // interface is running
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

void BaseInterface::close()
{
    setState(Interface::State::Disconnect);
    emit clearQueries();
}

void BaseInterface::reconnect()
{
    // Если устройство уже находится в состоянии переподключения,
    // то повторно заходить в цикл не следует.
    if (getState() != Interface::State::Reconnect)
    {
        setState(Interface::State::Reconnect);
        qCritical() << "Произошла ошибка соединения";
        while (getState() == Interface::State::Reconnect)
        {
            if (tryToReconnect())
                break;
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        if (getState() == Interface::State::Run)
        {
            emit reconnected();
            qCritical() << "Соединение восстановлено";
        }
    }
}
