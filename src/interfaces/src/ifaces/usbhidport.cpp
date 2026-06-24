#include "interfaces/ifaces/usbhidport.h"

#include <avm-gen/threadpool.h>
#include <hidapi/hidapi.h>

#include <QDebug>

constexpr int MaxSegmenthLength = 64; ///< Максимальная длина одного сегмента (0x40)
constexpr int hidApiErrorCode = -1;   ///< Код ошибки от HID API

UsbHidPort::UsbHidPort(UsbHidSettings *settings, QObject *parent)
    : BaseInterface("UsbHidPort", settings, parent)
    , m_hidDevice(nullptr)
    , m_settings(settings)
{
}

bool UsbHidPort::connect()
{
    if ((static_cast<u16>(m_settings->get("vendor_id")) == 0) || (static_cast<u16>(m_settings->get("product_id")) == 0))
    {
        qDebug() << Error::Msg::NoDeviceError;
        return false;
    }
    m_hidDevice = hid_open(m_settings->get("vendor_id"), m_settings->get("product_id"), //
        QString(m_settings->get("serial")).toStdWString().c_str());
    if (!m_hidDevice)
    {
        emit error(InterfaceError::OpenError);
        // Выводим ошибки только при первом подключении,
        // при реконнекте не логгируем эти события
        if (getState() == Interface::State::Connect)
        {
            hidErrorHandle();
            qDebug() << Error::Msg::OpenError;
        }
        return false;
    }
    else
    {
        if (getState() != Interface::State::Disconnect)
        {
            if (hid_set_nonblocking(m_hidDevice, 1) == hidApiErrorCode)
                hidErrorHandle();
            setState(Interface::State::Run);
            qInfo("Связь с устройством установлена");

            startTPolling();

            emit started();
            return true;
        }
        else
            return false;
    }
}

void UsbHidPort::disconnect()
{
    // 1. Останавливаем опрос
    if (m_pollTimer)
    {
        m_pollTimer->stop();
    }

    // 2. Чистим очередь
    m_readBuffer.clear();

    // 3. Закрываем порт
    if (m_hidDevice)
    {
        hid_close(m_hidDevice);
        m_hidDevice = nullptr;
    }

    emit clearQueries();
    emit finished();
}

QByteArray UsbHidPort::read(bool &status)
{
    if (!m_readBuffer.isEmpty())
    {
        status = true;
        return m_readBuffer.dequeue(); // Мгновенно забираем данные
    }

    status = false;
    return QByteArray();
}

bool UsbHidPort::write(const QByteArray &ba)
{
    auto copy(ba);
    auto status = writeDataToPort(copy);
    if (!status)
        emit clearQueries();
    return status;
}

bool UsbHidPort::writeDataToPort(QByteArray &command)
{
    if (command.size() > MaxSegmenthLength)
    {
        writeLog(Error::Msg::SizeError);
        qDebug() << Error::Msg::SizeError;
        return false;
    }

    if (command.size() < MaxSegmenthLength)
        command.append(MaxSegmenthLength - command.size(), static_cast<char>(0x00));
    command.prepend(static_cast<char>(0x00)); // Добавляем поле ID для HID protocol

    auto tmpt = static_cast<size_t>(command.size());
    auto writtenBytes = hid_write(m_hidDevice, reinterpret_cast<unsigned char *>(command.data()), tmpt);
    if (writtenBytes == hidApiErrorCode)
    {
        writeLog(Error::Msg::WriteError);
        hidErrorHandle();
        emit error(InterfaceError::WriteError);
        return false;
    }
    return true;
}

void UsbHidPort::hidErrorHandle()
{
    if (m_isLoggingEnabled)
    {
        auto errString = "HID API Error: " + QString::fromStdWString(hid_error(m_hidDevice));
        m_log.writeLog(Logger::Critical, errString);
    }
}

void UsbHidPort::startTPolling()
{
    if (!m_pollTimer)
    {
        m_pollTimer = new QTimer(this);
        QObject::connect(m_pollTimer, &QTimer::timeout, this, &UsbHidPort::checkForData);
    }
    // Запускаем опрос каждые 10 мс (для HID этого более чем достаточно)
    m_pollTimer->start(10);
}

void UsbHidPort::checkForData()
{
    if (!m_hidDevice)
        return;

    constexpr auto maxLength = MaxSegmenthLength + 1;
    QByteArray buffer(maxLength, 0);
    auto dataPtr = reinterpret_cast<unsigned char *>(buffer.data());

    // Читаем циклом, чтобы забрать ВСЕ пакеты, накопившиеся в буфере драйвера
    for (int readBytes = hid_read_timeout(m_hidDevice, dataPtr, maxLength, 0); readBytes != 0;
        readBytes = hid_read_timeout(m_hidDevice, dataPtr, maxLength, 0))
    {
        if (readBytes == hidApiErrorCode)
        {
            writeLog(Error::Msg::ReadError);
            hidErrorHandle();
            emit error(InterfaceError::ReadError);

            break;
        }

        buffer.resize(readBytes);
        m_readBuffer.enqueue(buffer);
        emit readyRead();
    }
}