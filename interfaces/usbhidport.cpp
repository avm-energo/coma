#include "usbhidport.h"

#include "../gen/board.h"
#include "../gen/helper.h"
#include "../gen/stdfunc.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMetaEnum>
#include <QTimer>
#include <array>
#ifdef _WIN32
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
    QString filename(metaObject()->className());
    filename.append(".").append(::logExt);
    log->Init(filename);
    log->WriteRaw(::logStart);
    m_hidDevice = nullptr;
    connect(this, &UsbHidPort::clearQueries, &UsbHidPort::clear);
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
#ifdef __linux__
    m_hidDevice = hid_open_path(deviceInfo().path.toStdString().c_str());
#endif
#ifdef _WIN32
    m_hidDevice = openDevice(m_deviceInfo);
#endif
    if (!m_hidDevice)
    {
        qDebug() << QString::fromWCharArray(hid_error(m_hidDevice));
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
    m_isShouldBeStopped = false;
    while (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        //_mutex.lock();
        //        if (isShouldBeStopped())
        //        {
        //_waiter.wait(&_mutex, 1000);
        //            continue;
        //        }
        //_mutex.unlock();
        // check if there's any data in input buffer
        if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
        {
            StdFunc::Wait(500);
            continue;
        }
        if (!m_hidDevice)
            continue;
        std::array<byte, HID::MaxSegmenthLength + 1> array; // +1 to ID
        bytes = hid_read(m_hidDevice, array.data(), HID::MaxSegmenthLength + 1);
        // Write
        if (bytes < 0)
        {
            // -1 is the only error value according to hidapi documentation.
            Q_ASSERT(bytes == -1);
            continue;
        }
        // Read
        if (bytes > 0)
        {
            QByteArray ba(reinterpret_cast<char *>(array.data()), bytes);
            writeLog(ba.toHex(), Direction::FromDevice);
            emit dataReceived(ba);
            m_waitForReply = false;
        }
        // QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (m_waitForReply)
            continue;
        // write data to port if there's something delayed in out queue
        checkQueue();
    }
    finish();
}

void UsbHidPort::deviceStateChanged(const UsbHidSettings &st, bool isConnected)
{
    if (!isConnected)
    {
        // Отключено другое устройство
        if (st != deviceInfo())
            return;
        else
        {
            // Отключено наше устройство
            shouldBeStopped();
            Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
            qInfo() << deviceInfo() << "disconnected";
            //            QTimer disconnectTimer;
            //            disconnectTimer.setSingleShot(true);
            //            disconnectTimer.setInterval(10000);
            //            connect(&disconnectTimer, &QTimer::timeout, [] {
            //                if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
            //                    Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
            //            });
        }
    }
    else
    {
        // Подключено другое устройство
        if (st != deviceInfo())
            return;
        // Устройство появляется не сразу после прихода события о его подключении
        StdFunc::Wait(100);
        emit clearQueries();
        if (!setupConnection())
            return;
        qInfo() << deviceInfo() << "connected";
        Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    }
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
    if (!m_hidDevice)
        return;

    QMutexLocker locker(&_mutex);
    m_writeQueue.clear();
    hid_close(m_hidDevice);
    m_hidDevice = nullptr;
}

void UsbHidPort::finish()
{
    closeConnection();
    qInfo() << metaObject()->className() << "finished";
    emit finished();
}

void UsbHidPort::clear()
{
    // QMutexLocker locker(&_mutex);
    m_waitForReply = false;
    m_writeQueue.clear();
}

bool UsbHidPort::isShouldBeStopped() const
{
    return m_isShouldBeStopped;
}

void UsbHidPort::shouldBeStopped()
{
    m_isShouldBeStopped = true;
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

bool UsbHidPort::writeData(const QByteArray &ba)
{
    QByteArray tba(ba);
    if (!m_hidDevice)
    {
        writeLog(Error::Msg::NoDeviceError);
        qCritical() << Error::Msg::NoDeviceError;
        return false;
    }

    if (tba.size() > HID::MaxSegmenthLength)
    {
        writeLog(Error::Msg::SizeError);
        qCritical() << Error::Msg::SizeError;
        return false;
    }

    if (tba.isEmpty())
    {
        writeLog(Error::Msg::NullDataError);
        qCritical() << Error::Msg::NullDataError;
        return false;
    }
    // NOTE
    writeLog(tba.toHex(), Proto::ToDevice);
    if (tba.size() < HID::MaxSegmenthLength)
        tba.append(HID::MaxSegmenthLength - tba.size(), static_cast<char>(0x00));

    // inserting ID field for HID protocol
    tba.prepend(static_cast<char>(0x00));

    size_t tmpt = static_cast<size_t>(tba.size());

    int errorCode = hid_write(m_hidDevice, reinterpret_cast<unsigned char *>(tba.data()), tmpt);
    if (errorCode == -1)
    {
        qCritical() << Error::Msg::WriteError;
        return false;
    }
    return true;
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
    // NOTE
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
    return true;
}

void UsbHidPort::checkQueue()
{
    // NOTE Реализовать
    // Q_ASSERT(Board::GetInstance().connectionState() == Board::ConnectionState::Connected);
    QMutexLocker locker(&_mutex);
    if (!m_writeQueue.isEmpty())
    {
        QByteArray ba = m_writeQueue.takeFirst();
        if (writeData(ba))
            m_waitForReply = true;
        else
            emit clearQueries();
    }
}

// FIXME Не реализовано
void UsbHidPort::usbStateChanged(void *message)
{
#ifdef _WIN32
    MSG *msg = static_cast<MSG *>(message);
    int msgType = msg->message;
    if (msgType != WM_DEVICECHANGE)
        return;

    switch (msg->wParam)
    {
    case DBT_CONFIGCHANGECANCELED:
        qDebug("DBT_CONFIGCHANGECANCELED");
        break;
    case DBT_CONFIGCHANGED:
        qDebug("DBT_CONFIGCHANGED");
        break;
    case DBT_CUSTOMEVENT:
        qDebug("DBT_CUSTOMEVENT");
        break;
    case DBT_DEVICEARRIVAL:
    {
        // DevicesFound();
        if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
        {
            //                if (m_devices.contains(m_usbWorker->deviceInfo()))
            //                {
            //                    m_devicePosition = m_devices.indexOf(m_usbWorker->deviceInfo());
            //                    m_usbWorker->setDeviceInfo(m_devices.at(m_devicePosition));
            //                    qDebug("Device arrived again");
            //                    if (!Reconnect())
            //                    {
            //                        qDebug("Reconnection failed");
            //                        Disconnect();
            //                    }
            //                }
        }
        break;
    }
    case DBT_DEVICEQUERYREMOVE:
        qDebug("DBT_DEVICEQUERYREMOVE");
        break;
    case DBT_DEVICEQUERYREMOVEFAILED:
        qDebug("DBT_DEVICEQUERYREMOVEFAILED");
        break;
    case DBT_DEVICEREMOVEPENDING:
        qDebug("DBT_DEVICEREMOVEPENDING");
        break;
    case DBT_DEVICEREMOVECOMPLETE:
    {
        qDebug("DBT_DEVICEREMOVECOMPLETE");
        // qDebug() << DevicesFound();
        if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
        {
            //                if (!m_devices.contains(m_usbWorker->deviceInfo()))
            //                {
            //                    qDebug() << "Device " << m_usbWorker->deviceInfo().serial << " removed
            //                    completely"; m_writeData.clear(); OutData.clear();
            //                    Finish(Error::Msg::NullDataError);
            //                    m_loop.exit();
            //                    QMessageBox::critical(nullptr, "Ошибка", "Связь с прибором была разорвана",
            //                    QMessageBox::Ok);
            //                }
        }
        break;
    }
    case DBT_DEVICETYPESPECIFIC:
        qDebug("DBT_DEVICETYPESPECIFIC");
        break;
    case DBT_QUERYCHANGECONFIG:
        qDebug("DBT_QUERYCHANGECONFIG");
        break;
    case DBT_DEVNODES_CHANGED:
    {
        qDebug("DBT_DEVNODES_CHANGED");
        // DevicesFound();

        // Ивенты должны происходить только если отключен подключенный раннее
        // прибор
        if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
        {
            //                if (!m_devices.contains(m_usbWorker->deviceInfo()))
            //                {
            //                    qDebug() << "Device " << m_usbWorker->deviceInfo().serial << " state changed";
            //                    Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
            //                }
        }
        break;
    }
    case DBT_USERDEFINED:
        qDebug("DBT_USERDEFINED");
        break;
    default:
        qDebug() << "Default";
        break;
    }

#endif
}
