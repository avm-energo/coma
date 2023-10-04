#include "interfaces/ports/usbhidport.h"

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

UsbHidPort::UsbHidPort(const UsbHidSettings &dev, QObject *parent)
    : BasePort("UsbHidPort", parent), m_deviceInfo(dev), m_hidDevice(nullptr), m_waitForReply(false)
{
    using namespace settings;
    QObject::connect(this, &BasePort::clearQueries, this, &UsbHidPort::clear);
    QSettings sets;
    missingCounterMax = sets.value(regMap[hidTimeout].name, "50").toInt();
}

bool UsbHidPort::connect()
{
    if ((m_deviceInfo.vendor_id == 0) || (m_deviceInfo.product_id == 0))
    {
        qCritical() << Error::Msg::NoDeviceError;
        closeConnection();
        return false;
    }
    m_hidDevice = hid_open(m_deviceInfo.vendor_id, m_deviceInfo.product_id, //
        m_deviceInfo.serial.toStdWString().c_str());
    if (!m_hidDevice)
    {
        hidErrorHandle();
        qCritical() << Error::Msg::OpenError;
        closeConnection();
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
    while (getReconnectLoopFlag())
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    connect();
    emit clearQueries();
}

QByteArray UsbHidPort::read(bool *status)
{
    constexpr auto maxLength = HID::MaxSegmenthLength + 1; // +1 to ID
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
        emit error(PortErrors::ReadError);
        // reconnectCycle();
        *status = false;
        data.clear();
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
        emit error(PortErrors::NoData);
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
    m_hidDevice = nullptr;
    m_dataGuard.unlock(); // unlock port
}

bool UsbHidPort::writeDataToPort(QByteArray &ba)
{
    if (!m_hidDevice)
    {
        writeLog(Error::Msg::NoDeviceError);
        qCritical() << Error::Msg::NoDeviceError;
        // closeConnection();
        return false;
    }
    if (ba.size() > HID::MaxSegmenthLength)
    {
        writeLog(Error::Msg::SizeError);
        qCritical() << Error::Msg::SizeError;
        return false;
    }

    if (ba.size() < HID::MaxSegmenthLength)
        ba.append(HID::MaxSegmenthLength - ba.size(), static_cast<char>(0x00));
    ba.prepend(static_cast<char>(0x00)); // inserting ID field for HID protocol

    auto tmpt = static_cast<size_t>(ba.size());
    int errorCode = hid_write(m_hidDevice, reinterpret_cast<unsigned char *>(ba.data()), tmpt); // write
    if (errorCode == -1)
    {
        writeLog(Error::Msg::WriteError);
        hidErrorHandle();
        emit error(PortErrors::WriteError);
        return false;
    }
    missingCounter = 0;
    return true;
}
