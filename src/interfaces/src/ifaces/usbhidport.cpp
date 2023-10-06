#include "interfaces/ifaces/usbhidport.h"

#include <QDebug>
#include <QSettings>
#include <array>
#include <gen/helper.h>
#include <gen/settings.h>
#include <hidapi/hidapi.h>

using Interface::Direction;
using Proto::CommandStruct;
using Proto::Starters;
using namespace Interface;

constexpr int MaxSegmenthLength = 64; // максимальная длина одного сегмента (0x40)

UsbHidPort::UsbHidPort(const UsbHidSettings &dev, QObject *parent)
    : BaseInterface("UsbHidPort", parent), m_deviceInfo(dev), m_hidDevice(nullptr), m_waitForReply(false)
{
    using namespace settings;
    QObject::connect(this, &BaseInterface::clearQueries, this, &UsbHidPort::clear);
    QSettings sets;
    missingCounterMax = sets.value(regMap[hidTimeout].name, "50").toInt();
}

bool UsbHidPort::connect()
{
    setState(State::Connect);
    if ((m_deviceInfo.vendor_id == 0) || (m_deviceInfo.product_id == 0))
    {
        qCritical() << Error::Msg::NoDeviceError;
        return false;
    }
    m_hidDevice = hid_open(m_deviceInfo.vendor_id, m_deviceInfo.product_id, //
        m_deviceInfo.serial.toStdWString().c_str());
    if (!m_hidDevice)
    {
        hidErrorHandle();
        qCritical() << Error::Msg::OpenError;
        return false;
    }
    else
    {
        if (getState() != Interface::State::Disconnect)
        {
            hid_set_nonblocking(m_hidDevice, 1);
            setState(State::Run);
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

void UsbHidPort::reconnect()
{
    // Если устройство уже находится в состоянии переподключения
    if (getState() != State::Reconnect)
    {
        setState(State::Reconnect);
        while (getReconnectLoopFlag())
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        setReconnectLoopFlag(true); // Восстанавливаем флаг цикла
        // Если во время реконнекта отключаемся от устройства,
        // то не предпринимаем попыток переподключиться.
        if (getState() != State::Disconnect)
        {
            disconnect();            // Закрываем текущее соединение
            StdFunc::Wait(1000);     // Устройство не сразу появляется на связи
            if (connect())           // Если успешно переподключились,
                emit clearQueries(); // то сбрасываем текущий запрос от парсера
            else // Если произошёл разрыв с устройством во время переподключения к нему,
                reconnect(); // то пытаемся переподключиться ещё раз
        }
    }
}

QByteArray UsbHidPort::read(bool *status)
{
    constexpr auto maxLength = MaxSegmenthLength + 1; // +1 to ID
    QByteArray data(maxLength, 0);
    auto dataPtr = reinterpret_cast<unsigned char *>(data.data());
    m_dataGuard.lock();                                                  // lock port
    auto bytes = hid_read_timeout(m_hidDevice, dataPtr, maxLength, 100); // read
    m_dataGuard.unlock();                                                // unlock port
    if (bytes < 0)
    {
        // -1 is the only error value according to hidapi documentation.
        Q_ASSERT(bytes == -1);
        writeLog(Error::Msg::ReadError);
        hidErrorHandle();
        emit error(InterfaceError::ReadError);
        *status = false;
        data.clear();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    // timeout from module (if avtuk accidentally couldnt response)
    if ((bytes == 0) && (m_waitForReply) && (missingCounter == missingCounterMax))
    {
        constexpr int responseSize = 5;
        unsigned char timeoutResponse[responseSize] = { 0x3c, 0xf0, 0x01, 0x00, 0x01 };
        data.prepend(reinterpret_cast<char *>(&timeoutResponse[0]), responseSize);
        bytes = responseSize;
    }
    if (bytes > 0)
    {
        data.resize(bytes);
        m_waitForReply = false;
        missingCounter = 0;
    }
    else
    {
        data.clear();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    return data;
}

bool UsbHidPort::write(const QByteArray &ba)
{
    m_currCommand = ba;
    auto status = writeDataToPort(m_currCommand);
    if (status)
        m_waitForReply = true;
    else
        emit clearQueries();
    return status;
}

void UsbHidPort::hidErrorHandle()
{
    auto errString = "HID API Error: " + QString::fromStdWString(hid_error(m_hidDevice));
    qCritical() << errString;
    writeLog(errString.toLocal8Bit());
}

const UsbHidSettings &UsbHidPort::deviceInfo() const
{
    return m_deviceInfo;
}

void UsbHidPort::clear()
{
    m_dataGuard.lock(); // lock port
    m_waitForReply = false;
    m_currCommand.clear();
    m_dataGuard.unlock(); // unlock port
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
    int errorCode = hid_write(m_hidDevice, reinterpret_cast<unsigned char *>(command.data()), tmpt);
    if (errorCode == -1)
    {
        writeLog(Error::Msg::WriteError);
        hidErrorHandle();
        emit error(InterfaceError::WriteError);
        return false;
    }
    missingCounter = 0;
    return true;
}
