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
        hidErrorHandle();
        emit error(PortErrors::ReadError);
        reconnect();
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

// void UsbHidPort::deviceConnected(const UsbHidSettings &st)
//{
//    QElapsedTimer timer;
//    timer.start();
//    // Подключено другое устройство
//    if (st != deviceInfo())
//        return;
//    qDebug() << timer.elapsed();
//    // Устройство появляется не сразу после прихода события о его подключении
//    StdFunc::Wait(100);
//    emit clearQueries();
//    qDebug() << timer.elapsed();
//    if (!connect())
//        return;
//    qDebug() << timer.elapsed();
//    qInfo() << deviceInfo() << "connected";
//    emit stateChanged(State::Run);
//    qDebug() << timer.elapsed();
//}

// void UsbHidPort::deviceDisconnected(const UsbHidSettings &st)
//{
//    // Отключено другое устройство
//    if (st != deviceInfo())
//        return;
//    // Отключено наше устройство
//    emit stateChanged(State::Disconnect);
//    qInfo() << deviceInfo() << "disconnected";
//}

// void UsbHidPort::deviceConnected()
//{
//    if (!connect())
//        return;
//    qInfo() << deviceInfo() << "connected";
//}

// void UsbHidPort::deviceDisconnected()
//{
//    // Отключено наше устройство
//    emit stateChanged(State::Disconnect);
//    qInfo() << deviceInfo() << "disconnected";
//}

// bool UsbHidPort::shouldBeStopped() const
//{
//    return m_shouldBeStopped;
//}

// void UsbHidPort::shouldBeStopped(bool isShouldBeStopped)
//{
//    m_shouldBeStopped = isShouldBeStopped;
//}

// void UsbHidPort::setDeviceInfo(const UsbHidSettings &deviceInfo)
//{
//    m_deviceInfo = deviceInfo;
//}

// void UsbHidPort::usbEvent(const USBMessage message, quint32 type)
//{
//#ifdef Q_OS_WINDOWS
//    qDebug() << message.guid << message.type;
//    switch (type)
//    {
//    case DBT_DEVICEARRIVAL:
//    {
//        if (message.type != DBT_DEVTYP_DEVICEINTERFACE)
//            break;
//        QRegularExpression regex(HID::headerValidator);
//        QRegularExpressionMatch match = regex.match(message.guid);
//        if (!match.hasMatch())
//            break;
//        if (match.captured(0) != "USB" && match.captured(0) != "HID")
//            break;
//        if (deviceInfo().hasMatch(message.guid))
//        {
//            if (!shouldBeStopped())
//                deviceConnected();
//            shouldBeStopped(false);
//            break;
//        }
//        if (deviceInfo().hasPartialMatch(message.guid))
//        {
//            if (!shouldBeStopped())
//                deviceConnected();
//            shouldBeStopped(false);
//            break;
//        }
//        break;
//    }
//    case DBT_DEVICEREMOVECOMPLETE:
//    {
//        if (message.type != DBT_DEVTYP_DEVICEINTERFACE)
//            break;
//        QRegularExpression regex(HID::headerValidator);
//        QRegularExpressionMatch match = regex.match(message.guid);
//        if (!match.hasMatch())
//            break;
//        if (match.captured(0) != "USB" && match.captured(0) != "HID")
//            break;
//        if (deviceInfo().hasMatch(message.guid))
//        {
//            if (shouldBeStopped())
//                deviceDisconnected();
//            shouldBeStopped(true);
//            break;
//        }
//        if (deviceInfo().hasPartialMatch(message.guid))
//        {
//            if (shouldBeStopped())
//                deviceDisconnected();
//            shouldBeStopped(true);
//            break;
//        }
//        break;
//    }
//    case DBT_DEVNODES_CHANGED:
//    {
//        // NOTE Игнорируем события изменения состояния.
//        // Можно как-то обрабатывать.
//        // Приходят перед и после обрабатываемых событий.
//        // Внутри не содержат ничего, получаем только тип
//        // события.
//        break;
//    }
//    default:
//        qInfo() << "Unhadled case" << QString::number(type, 16);
//    }
//#else
//    Q_UNUSED(message)
//    Q_UNUSED(type)
//#endif
//}

// void UsbHidPort::usbEvent(const QString &guid, quint32 msgType)
//{
//#ifdef Q_OS_WINDOWS
//    if (m_deviceInfo.hasMatch(guid) || m_deviceInfo.hasPartialMatch(guid))
//    {
//        switch (msgType)
//        {
//        case DBT_DEVICEARRIVAL:
//            if (connect())
//                qInfo() << m_deviceInfo << " connected";
//            break;
//        case DBT_DEVICEREMOVECOMPLETE:
//            closeConnection();
//            qInfo() << m_deviceInfo << " disconnected";
//            break;
//        case DBT_DEVNODES_CHANGED:
//            // Ignore
//            break;
//        default:
//            qInfo() << "Unhadled case" << QString::number(msgType, 16);
//        }
//    }
//#else
//    Q_UNUSED(guid)
//    Q_UNUSED(msgType)
//#endif
//}

bool UsbHidPort::writeDataToPort(QByteArray &ba)
{
    if (!m_hidDevice)
    {
        writeLog(Error::Msg::NoDeviceError);
        qCritical() << Error::Msg::NoDeviceError;
        closeConnection();
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
        hidErrorHandle();
        return false;
    }
    missingCounter = 0;
    return true;
}
