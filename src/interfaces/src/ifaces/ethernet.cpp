#include "interfaces/ifaces/ethernet.h"

#include <QDebug>
#include <QEventLoop>
#include <QNetworkProxy>
#include <QTimer>

Ethernet::Ethernet(IEC104Settings *settings, QObject *parent)
    : BaseInterface("Ethernet", settings, parent)
    , m_settings(settings)
    , m_socket(new QTcpSocket(this))
{
    m_socket->setProxy(QNetworkProxy::NoProxy);
    QObject::connect(m_socket, &QAbstractSocket::stateChanged,  //
        this, &Ethernet::handleSocketState, Qt::DirectConnection);
    QObject::connect(m_socket, &QAbstractSocket::errorOccurred, //
        this, &Ethernet::handleSocketError, Qt::DirectConnection);
    QObject::connect(m_socket, &QTcpSocket::readyRead, this, &BaseInterface::readyRead);
}

bool Ethernet::connect()
{
    m_socket->connectToHost(
        m_settings->get("ip"), m_settings->get("port"), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);

    // Ждём завершения connectToHost() локальным event loop'ом (не waitForConnected()),
    // чтобы не блокировать основной цикл событий (GUI) на время подключения.
    QEventLoop loop;
    bool timedOut = false;
    QObject::connect(m_socket, &QAbstractSocket::connected, &loop, &QEventLoop::quit);
    QObject::connect(m_socket, &QAbstractSocket::errorOccurred, &loop, &QEventLoop::quit);
    QTimer::singleShot(m_settings->get<int>("connectTimeout"), &loop,
        [&loop, &timedOut]
        {
            timedOut = true;
            loop.quit();
        });
    // ExcludeUserInputEvents: этот event loop крутится внутри синхронного вызова, начатого
    // диалогом настройки соединения (см. InterfaceEthernetDialog::setInterface()). Если позволить
    // обрабатывать клики мыши/клавиатуру здесь, пользователь успевает закрыть этот диалог
    // (Qt::WA_DeleteOnClose) прямо во время ожидания подключения — тогда после выхода из этого
    // цикла управление вернётся в метод уже уничтоженного объекта диалога (use-after-free).
    // Мышь/клавиатура просто откладываются и будут доставлены как обычно после выхода отсюда.
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    if (m_socket->state() != QAbstractSocket::ConnectedState)
    {
        if (timedOut)
        {
            m_log.writeLog(Logger::Critical, "Connect timeout");
            m_socket->abort();
            emit error(InterfaceError::OpenError);
        }
        return false;
    }

    if (getState() != Interface::State::Disconnect)
    {
        setState(Interface::State::Run);
        qDebug("Связь с устройством установлена");
        emit started();
        return true;
    }
    return false;
}

void Ethernet::disconnect()
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState)
    {
        // Мягко просим отключится + переводим openmode == false
        m_socket->close();

        if (m_socket->state() == QAbstractSocket::UnconnectedState
            || m_socket->waitForDisconnected(m_settings->get<int>("disconnectTimeout")))
            m_log.writeLog(Logger::Info, "Socket disconnected");
        else
        {
            m_log.writeLog(
                Logger::Warning, QString("Disconnect error: %1. Connection will abort").arg(m_socket->errorString()));
            m_socket->abort();
        }
    }

    emit disconnected();
}

// Blocking data reading from the socket
QByteArray Ethernet::read(bool &status)
{
    QByteArray data;

    if (m_socket->bytesAvailable())
        status = true;

    while (m_socket->bytesAvailable())
    {
        data += m_socket->readAll();
    }

    return data;
}

bool Ethernet::write(const QByteArray &data)
{
    if (!m_socket->isOpen() || !m_socket->isWritable())
    {
        m_log.writeLog(Logger::Critical, "Ethernet write data to the closed socket");
        return false;
    }
    m_dataGuard.lock();                 // lock ethernet iface
    auto bytes = m_socket->write(data); // write data
    m_dataGuard.unlock();               // unlock ethernet iface
    if (bytes <= 0)
    {
        qDebug() << "Ошибка записи данных в сокет";
        emit error(InterfaceError::WriteError);
        return false;
    }
    return true;
}

void Ethernet::handleSocketState(const QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::SocketState::UnconnectedState:
        m_log.writeLog(Logger::Info, "Socket unconnected");
        break;
    case QAbstractSocket::SocketState::HostLookupState:
        m_log.writeLog(Logger::Info, "Socket enters host lookup state");
        break;
    case QAbstractSocket::SocketState::ConnectingState:
        m_log.writeLog(Logger::Info, "Socket enters connecting state");
        break;
    case QAbstractSocket::SocketState::ConnectedState:
        m_log.writeLog(Logger::Info, "Socket connected!");
        break;
    case QAbstractSocket::SocketState::BoundState:
        m_log.writeLog(Logger::Info, "Socket is bound to address and port");
        break;
    case QAbstractSocket::SocketState::ClosingState:
        m_log.writeLog(Logger::Info, "Socket is in closing state");
        break;
    case QAbstractSocket::SocketState::ListeningState:
        m_log.writeLog(Logger::Info, "Socket is in listening state");
        break;
    default:
        m_log.writeLog(Logger::Warning, "Unprocessed state");
        break;
    }
}

void Ethernet::handleSocketError(const QAbstractSocket::SocketError err)
{
    switch (err)
    {
    case QAbstractSocket::SocketError::SocketTimeoutError:
        // ignore
        break;
    case QAbstractSocket::SocketError::RemoteHostClosedError:
        // Устройство разорвало УЖЕ установленное соединение (например, ушло в ResetSystem() после
        // записи конфигурации/ВПО) - это обрыв рабочего соединения, а не ошибка его первичного
        // открытия. OpenError вне самого первого подключения молча игнорируется в
        // ConnectionManager::handleInterfaceErrors (см. m_isInitial) - если завести такой разрыв
        // туда же, реконнект узнаёт о нём только через прикладные таймауты запросов, что даёт лишние
        // секунды простоя. ReadError же корректно считается и запускает переподключение.
        m_log.writeLog(Logger::Warning, m_socket->errorString());
        emit error(InterfaceError::ReadError);
        break;
    default:
        m_log.writeLog(Logger::Critical, m_socket->errorString());
        emit error(InterfaceError::OpenError);
        break;
    }
}
