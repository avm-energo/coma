#include "interfaces/ifaces/baseinterface.h"

#include <libavm-gen/settings.h>

#include <QCoreApplication>
#include <QElapsedTimer>

BaseInterface::BaseInterface(const QString &logFilename, BaseSettings *settings, QObject *parent)
    : QObject(parent)
    , m_state(Interface::State::Connect)
{
    m_reconnectInterval = settings->get("reconnectInterval");
    qRegisterMetaType<InterfaceError>();
    m_isLoggingEnabled = settings->get("isLoggingEnabled");
    m_log.setLogLevel(settings->get("logLevel"));
    m_log.writeStart(logFilename + ".log");
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
    if (m_isLoggingEnabled)
    {
        QString msg = metaObject()->className();
        switch (dir)
        {
        case Interface::FromDevice:
            msg += ": -> ";
            break;
        case Interface::ToDevice:
            msg += ": <- ";
            break;
        default:
            msg += ": ";
            break;
        }
        msg += ba;
        m_log.writeLog(Logger::MessageTypes::Debug, msg);
    }
}

void BaseInterface::writeLog(const Error::Msg msg)
{
    if (m_isLoggingEnabled)
        m_log.writeLog(Logger::MessageTypes::Critical, QVariant::fromValue(msg).toString());
}

void BaseInterface::poll()
{   
    Interface::State state = getState();
    if (state != Interface::State::Run)
        return;

    bool status = true;
    QByteArray data = read(status);

    if (!data.isEmpty() && status)
    {
        writeLog(data.toHex(), Interface::Direction::FromDevice);
        emit dataReceived(data);
        emit executorWakeUp();
    }
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
        qDebug() << status;
    }
}

void BaseInterface::close()
{
    setState(Interface::State::Disconnect);
    emit clearQueries();

    disconnect();
    emit finished(); // Проверить насколько безопасно перемещать сюда из poll
    m_log.writeLog(Logger::MessageTypes::Info, QString(metaObject()->className()) + " is finished\n");
}

void BaseInterface::reconnect()
{
    // Если устройство уже находится в состоянии переподключения,
    // то повторно заходить в цикл не следует.
    if (getState() != Interface::State::Reconnect)
    {
        setState(Interface::State::Reconnect);
        while (getState() == Interface::State::Reconnect)
        {
            disconnect();                       // Закрываем текущее соединение
            StdFunc::Wait(m_reconnectInterval); // Интервал между закрытием подключения и попыткой переподключиться
            if (connect())                      // Пытаемся подключиться к интерфейсу заново
                break;
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        if (getState() == Interface::State::Run)
        {
            emit reconnected();
        }
    }
}
