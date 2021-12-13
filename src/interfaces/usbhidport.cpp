#include "usbhidport.h"

#include "../gen/helper.h"
#include "../gen/stdfunc.h"
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
#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif
using Proto::CommandStruct;
using Proto::Direction;
using Proto::Starters;
UsbHidPort::UsbHidPort(const UsbHidSettings &dev, LogClass *logh, QObject *parent)
    : QObject(parent), log(logh), m_deviceInfo(dev)
{
    QString filename("UsbHidPort");
    filename.append(".").append(::logExt);
    log->Init(filename);
    log->WriteRaw(::logStart);
    m_hidDevice = nullptr;
    m_shouldBeStopped = false;
    connect(this, &UsbHidPort::clearQueries, &UsbHidPort::clear);
    QSettings sets;
    missingCounterMax = sets.value(settings::hidTimeout, "50").toInt();
}

UsbHidPort::~UsbHidPort()
{
}

inline hid_device *openDevice(const UsbHidSettings &dev)
{
    return hid_open(dev.vendor_id, dev.product_id, dev.serial.toStdWString().c_str());
}

bool UsbHidPort::setupConnection()
{
    if ((deviceInfo().vendor_id == 0) || (deviceInfo().product_id == 0))
    {
        qCritical() << Error::Msg::NoDeviceError;
        finish();
        return false;
    }

    m_hidDevice = openDevice(m_deviceInfo);

    if (!m_hidDevice)
    {
        qCritical() << Error::Msg::OpenError;
        finish();
        return false;
    }
    hid_set_nonblocking(m_hidDevice, 1);
    qInfo("HID opened successfully");
    return true;
}

void UsbHidPort::poll()
{
    int bytes;
    m_waitForReply = false;
    const auto &iface = BaseInterface::iface();
    while (iface->state() != BaseInterface::State::Stop)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);

        // check if there's any data in input buffer
        if (iface->state() == BaseInterface::State::Wait)
        {
            StdFunc::Wait(500);
            continue;
        }
        if (!m_hidDevice)
            continue;
        std::array<byte, HID::MaxSegmenthLength + 1> array; // +1 to ID
        bytes = hid_read_timeout(m_hidDevice, array.data(), HID::MaxSegmenthLength + 1, 100);
        // Write
        if (bytes < 0)
        {
            // -1 is the only error value according to hidapi documentation.
            Q_ASSERT(bytes == -1);
            continue;
        }
        // Read
        QByteArray ba;
        // timeout from module (if avtuk accidentally couldnt response)
        if ((bytes == 0) && (m_waitForReply) && (missingCounter == missingCounterMax))
        {
            array = { 0x3c, 0xf0, 0x01, 0x00, 0x01 };
            bytes = 5;
            m_waitForReply = false;
            missingCounter = 0;
        }
        if (bytes > 0)
        {
            ba = QByteArray(reinterpret_cast<char *>(array.data()), bytes);
            m_waitForReply = false;
            missingCounter = 0;
        }
        if (!ba.isEmpty())
        {
            missingCounter = 0;
            writeLog(ba.toHex(), Direction::FromDevice);
            emit dataReceived(ba);
        }
        if (m_waitForReply)
        {
            ++missingCounter;
            continue;
        }
        // write data to port if there's something delayed in out queue
        if (checkQueue())
        {
            missingCounter = 0;
        }
    }
    finish();
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
    if (!setupConnection())
        return;
    qDebug() << timer.elapsed();
    qInfo() << deviceInfo() << "connected";
    emit stateChanged(BaseInterface::State::Run);
    qDebug() << timer.elapsed();
}

void UsbHidPort::deviceDisconnected(const UsbHidSettings &st)
{
    // Отключено другое устройство
    if (st != deviceInfo())
        return;
    // Отключено наше устройство
    emit stateChanged(BaseInterface::State::Wait);
    qInfo() << deviceInfo() << "disconnected";
}

void UsbHidPort::deviceConnected()
{
    if (!setupConnection())
        return;
    qInfo() << deviceInfo() << "connected";
    emit stateChanged(BaseInterface::State::Run);
}

void UsbHidPort::deviceDisconnected()
{
    // Отключено наше устройство
    emit stateChanged(BaseInterface::State::Wait);
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

void UsbHidPort::writeDataAttempt(const QByteArray &ba)
{
    QMutexLocker locker(&_mutex);
    m_writeQueue.append(ba);
}

void UsbHidPort::closeConnection()
{
    if (m_hidDevice)
        hid_close(m_hidDevice);

    clear();
}

void UsbHidPort::finish()
{
    closeConnection();
    qInfo() << metaObject()->className() << "finished";
    emit finished();
    QCoreApplication::processEvents();
}

void UsbHidPort::clear()
{
    QMutexLocker locker(&_mutex);
    m_waitForReply = false;
    m_writeQueue.clear();
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
#endif
}

void UsbHidPort::writeLog(QByteArray ba, Direction dir)
{
#ifdef HIDUSB_LOG
    QByteArray tmpba = QByteArray(metaObject()->className());
    switch (dir)
    {
    case Proto::FromDevice:
        tmpba.append(": -> ");
        break;
    case Proto::ToDevice:
        tmpba.append(": <- ");
        break;
    default:
        tmpba.append(":  ");
        break;
    }
    tmpba.append(ba).append("\n");
    log->WriteRaw(tmpba);
#endif
}

bool UsbHidPort::writeData(QByteArray &ba)
{
    if (!m_hidDevice)
    {
        writeLog(Error::Msg::NoDeviceError);
        qCritical() << Error::Msg::NoDeviceError;
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
    writeLog(ba.toHex(), Proto::ToDevice);
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

bool UsbHidPort::checkQueue()
{
    QMutexLocker locker(&_mutex);
    if (m_writeQueue.isEmpty())
        return false;
    QByteArray ba = m_writeQueue.takeFirst();
    locker.unlock();
    if (writeData(ba))
        return m_waitForReply = true;
    else
        emit clearQueries();
    return false;
}
