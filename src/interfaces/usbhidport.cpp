#include "usbhidport.h"

#include "baseinterface.h"
#include "hidapi/hidapi.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMetaEnum>
#include <QRegularExpression>
#include <QSettings>
#include <QTimer>
#include <array>
#include <gen/helper.h>
#include <gen/settings.h>
#include <gen/stdfunc.h>

#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

using Interface::Direction;
using Proto::CommandStruct;
using Proto::Starters;
using namespace Interface;

UsbHidPort::UsbHidPort(const UsbHidSettings &dev, QObject *parent)
    : BasePort("UsbHidPort", parent)
    , m_deviceInfo(dev)
    , m_hidDevice(nullptr)
    , m_waitForReply(false)
    , m_shouldBeStopped(false)
{
    using namespace settings;
    QObject::connect(this, &UsbHidPort::clearQueries, &UsbHidPort::clear);
    QSettings sets;
    missingCounterMax = sets.value(regMap[hidTimeout].name, "50").toInt();
}

// inline hid_device *openDevice(const UsbHidSettings &dev)
//{
//    return hid_open(dev.vendor_id, dev.product_id, dev.serial.toStdWString().c_str());
//}

bool UsbHidPort::connect()
{
    if ((deviceInfo().vendor_id == 0) || (deviceInfo().product_id == 0))
    {
        qCritical() << Error::Msg::NoDeviceError;
        closeConnection();
        return false;
    }
    m_hidDevice = hid_open(m_deviceInfo.vendor_id, m_deviceInfo.product_id, //
        m_deviceInfo.serial.toStdWString().c_str());
    if (!m_hidDevice)
    {
        qCritical() << Error::Msg::OpenError;
        closeConnection();
        return false;
    }
    else
    {
        hid_set_nonblocking(m_hidDevice, 1);
        setState(State::Run);
        qInfo("HID opened successfully");
        return true;
    }
}

QByteArray UsbHidPort::read(bool *status)
{
    constexpr auto maxLength = HID::MaxSegmenthLength + 1; // +1 to ID
    QByteArray data(maxLength, 0);
    auto dataPtr = reinterpret_cast<unsigned char *>(data.data());
    auto bytes = hid_read_timeout(m_hidDevice, dataPtr, maxLength, 100);
    if (bytes < 0)
    {
        // -1 is the only error value according to hidapi documentation.
        Q_ASSERT(bytes == -1);
        emit error(ReadError);
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
        emit error(NoData);
    }

    return data;
}

void UsbHidPort::deviceConnected(const UsbHidSettings &st)
{
    QElapsedTimer timer;
    timer.start();
    // Подключено другое устройство
    if (st != deviceInfo())
        return;
    qDebug() << timer.elapsed();
    // Устройство появляется не сразу после прихода события о его подключении
    StdFunc::Wait(100);
    emit clearQueries();
    qDebug() << timer.elapsed();
    if (!connect())
        return;
    qDebug() << timer.elapsed();
    qInfo() << deviceInfo() << "connected";
    emit stateChanged(State::Run);
    qDebug() << timer.elapsed();
}

void UsbHidPort::deviceDisconnected(const UsbHidSettings &st)
{
    // Отключено другое устройство
    if (st != deviceInfo())
        return;
    // Отключено наше устройство
    emit stateChanged(State::Disconnect);
    qInfo() << deviceInfo() << "disconnected";
}

void UsbHidPort::deviceConnected()
{
    if (!connect())
        return;
    qInfo() << deviceInfo() << "connected";
    emit stateChanged(State::Run);
}

void UsbHidPort::deviceDisconnected()
{
    // Отключено наше устройство
    emit stateChanged(State::Disconnect);
    qInfo() << deviceInfo() << "disconnected";
    emit clearQueries();
}

bool UsbHidPort::shouldBeStopped() const
{
    return m_shouldBeStopped;
}

void UsbHidPort::shouldBeStopped(bool isShouldBeStopped)
{
    m_shouldBeStopped = isShouldBeStopped;
}

UsbHidSettings UsbHidPort::deviceInfo() const
{
    return m_deviceInfo;
}

void UsbHidPort::setDeviceInfo(const UsbHidSettings &deviceInfo)
{
    m_deviceInfo = deviceInfo;
}

bool UsbHidPort::writeData(const QByteArray &ba)
{
    QMutexLocker locker(&_mutex);
    m_currCommand = ba;
    if (!m_currCommand.isEmpty())
    {
        auto status = writeDataToPort(m_currCommand);
        if (status)
            return m_waitForReply = true;
        else
            emit clearQueries();
    }
    return false;
}

void UsbHidPort::disconnect()
{
    if (m_hidDevice)
        hid_close(m_hidDevice);
    emit clearQueries();
}

void UsbHidPort::clear()
{
    QMutexLocker locker(&_mutex);
    m_waitForReply = false;
    m_currCommand.clear();
    m_hidDevice = nullptr;
}

void UsbHidPort::usbEvent(const USBMessage message, quint32 type)
{
#ifdef Q_OS_WINDOWS
    qDebug() << message.guid << message.type;
    switch (type)
    {

    case DBT_DEVICEARRIVAL:
    {

        if (message.type != DBT_DEVTYP_DEVICEINTERFACE)
            break;
        QRegularExpression regex(HID::headerValidator);
        QRegularExpressionMatch match = regex.match(message.guid);
        if (!match.hasMatch())
            break;
        if (match.captured(0) != "USB" && match.captured(0) != "HID")
            break;
        if (deviceInfo().hasMatch(message.guid))
        {
            if (!shouldBeStopped())
                deviceConnected();
            shouldBeStopped(false);
            break;
        }
        if (deviceInfo().hasPartialMatch(message.guid))
        {
            if (!shouldBeStopped())
                deviceConnected();
            shouldBeStopped(false);
            break;
        }

        break;
    }
    case DBT_DEVICEREMOVECOMPLETE:
    {

        if (message.type != DBT_DEVTYP_DEVICEINTERFACE)
        {
            return;
        }
        QRegularExpression regex(HID::headerValidator);
        QRegularExpressionMatch match = regex.match(message.guid);
        if (!match.hasMatch())
            break;
        if (match.captured(0) != "USB" && match.captured(0) != "HID")
            break;

        if (deviceInfo().hasMatch(message.guid))
        {
            if (shouldBeStopped())
                deviceDisconnected();
            shouldBeStopped(true);
            break;
        }
        if (deviceInfo().hasPartialMatch(message.guid))
        {
            if (shouldBeStopped())
                deviceDisconnected();
            shouldBeStopped(true);
            break;
        }

        break;
    }
    case DBT_DEVNODES_CHANGED:
    {
        // NOTE Игнорируем события изменения состояния.
        // Можно как-то обрабатывать.
        // Приходят перед и после обрабатываемых событий.
        // Внутри не содержат ничего, получаем только тип
        // события.
        break;
    }
    default:
        qInfo() << "Unhadled case" << QString::number(type, 16);
    }
#else
    Q_UNUSED(message)
    Q_UNUSED(type)
#endif
}

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

    if (ba.isEmpty())
    {
        writeLog(Error::Msg::NullDataError);
        qCritical() << Error::Msg::NullDataError;
        return false;
    }
    writeLog(ba.toHex(), Interface::ToDevice);
    if (ba.size() < HID::MaxSegmenthLength)
        ba.append(HID::MaxSegmenthLength - ba.size(), static_cast<char>(0x00));

    // inserting ID field for HID protocol
    ba.prepend(static_cast<char>(0x00));

    size_t tmpt = static_cast<size_t>(ba.size());

    int errorCode = hid_write(m_hidDevice, reinterpret_cast<unsigned char *>(ba.data()), tmpt);
    if (errorCode == -1)
    {
        qCritical() << Error::Msg::WriteError;
        return false;
    }
    missingCounter = 0;
    return true;
}
