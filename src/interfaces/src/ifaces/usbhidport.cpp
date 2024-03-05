#include "interfaces/ifaces/usbhidport.h"

#include <QDebug>
#include <hidapi/hidapi.h>

constexpr int MaxSegmenthLength = 64; ///< Максимальная длина одного сегмента (0x40)
constexpr int hidApiErrorCode = -1;   ///< Код ошибки от HID API

UsbHidPort::UsbHidPort(const UsbHidSettings &settings, QObject *parent)
    : BaseInterface("UsbHidPort", parent), m_settings(settings), m_hidDevice(nullptr)
{
}

bool UsbHidPort::connect()
{
    if ((m_settings.vendor_id == 0) || (m_settings.product_id == 0))
    {
        qCritical() << Error::Msg::NoDeviceError;
        return false;
    }
    m_hidDevice = hid_open(m_settings.vendor_id, m_settings.product_id, //
        m_settings.serial.toStdWString().c_str());
    if (!m_hidDevice)
    {
        emit error(InterfaceError::OpenError);
        // Выводим ошибки только при первом подключении,
        // при реконнекте не логгируем эти события
        if (getState() == Interface::State::Connect)
        {
            hidErrorHandle();
            qCritical() << Error::Msg::OpenError;
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
            qInfo("HID opened successfully");
            emit started();
            return true;
        }
        else
            return false;
    }
}

void UsbHidPort::disconnect()
{
    if (m_hidDevice)
        hid_close(m_hidDevice);
    emit clearQueries();
}

QByteArray UsbHidPort::read(bool &status)
{
    constexpr auto maxLength = MaxSegmenthLength + 1; // +1 for ID
    QByteArray data(maxLength, 0);
    auto dataPtr = reinterpret_cast<unsigned char *>(data.data());
    m_dataGuard.lock();                                                      // lock iface
    auto readBytes = hid_read_timeout(m_hidDevice, dataPtr, maxLength, 100); // read
    m_dataGuard.unlock();                                                    // unlock iface
    if (readBytes == hidApiErrorCode)
    {
        // -1 is the only error value according to hidapi documentation.
        writeLog(Error::Msg::ReadError);
        hidErrorHandle();
        emit error(InterfaceError::ReadError);
        data.clear();
        status = false;
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    else if (readBytes > 0)
    {
        data.resize(readBytes);
        status = true;
    }
    else
    {
        data.clear();
        status = false;
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    return data;
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
        qCritical() << Error::Msg::SizeError;
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
    auto errString = "HID API Error: " + QString::fromStdWString(hid_error(m_hidDevice));
    writeLog(errString.toLocal8Bit());
}

const UsbHidSettings &UsbHidPort::deviceInfo() const
{
    return m_settings;
}
