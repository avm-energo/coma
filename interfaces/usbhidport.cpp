#include "usbhidport.h"

#include "../gen/board.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QMetaEnum>
#include <array>

#ifdef _WIN32
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

UsbHidPort::UsbHidPort(const DeviceConnectStruct &dev, LogClass *logh, QObject *parent)
    : QObject(parent), log(logh), m_deviceInfo(dev)
{
    m_hidDevice = nullptr;
}

UsbHidPort::~UsbHidPort()
{
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
    HidDevice = hid_open_path(deviceInfo().path.toStdString().c_str());
#endif
#ifdef _WIN32
    m_hidDevice = hid_open(deviceInfo().vendor_id, deviceInfo().product_id, deviceInfo().serial.toStdWString().c_str());
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
    std::array<byte, HID::MaxSegmenthLength + 1> array; // +1 to ID
    while (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
    {
        // check if there's any data in input buffer
        if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
            continue;
        if (m_hidDevice)
        {
            bytes = hid_read_timeout(m_hidDevice, array.data(), HID::MaxSegmenthLength + 1, HID::MainLoopDelay);
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
                emit dataReceived(ba);
            }
            // write data to port if there's something delayed in out queue
            checkQueue();
        }
    }
    finish();
}

DeviceConnectStruct UsbHidPort::deviceInfo() const
{
    return m_deviceInfo;
}

void UsbHidPort::setDeviceInfo(DeviceConnectStruct deviceInfo)
{
    m_deviceInfo = deviceInfo;
}

void UsbHidPort::writeDataAttempt(QByteArray &ba)
{
    QMutexLocker locker(&mutex_);
    m_writeQueue.append(ba);
}

void UsbHidPort::closeConnection()
{
    if (!m_hidDevice)
        return;

    QMutexLocker locker(&mutex_);
    m_writeQueue.clear();
    hid_close(m_hidDevice);
    m_hidDevice = nullptr;
}

void UsbHidPort::finish()
{
    closeConnection();
    qInfo("UThread finished");
    emit finished();
}

void UsbHidPort::writeLog(QByteArray ba, Direction dir)
{
#ifdef HIDUSB_LOG
    QByteArray tmpba = QByteArray(metaObject()->className());
    switch (dir)
    {
    case FromDevice:
        tmpba.append(": <-");
    case ToDevice:
        tmpba.append(": ->");
    default:
        tmpba.append(":  ");
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

    if (ba.size() < HID::MaxSegmenthLength)
        ba.append(HID::MaxSegmenthLength - ba.size(), static_cast<char>(0x00));

    // inserting ID field
    ba.prepend(static_cast<char>(0x00));
    writeLog(ba.toHex(), ToDevice);
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
    QMutexLocker locker(&mutex_);
    if (!m_writeQueue.isEmpty())
    {
        QByteArray ba = m_writeQueue.takeFirst();
        writeData(ba);
    }
}

QList<DeviceConnectStruct> UsbHidPort::devicesFound(quint16 vid)
{
    hid_device_info *devs, *cur_dev;
    // Enumerate all, extract only needed
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    QList<DeviceConnectStruct> sl;
    while (cur_dev)
    {
        if (cur_dev->vendor_id == vid)
        {
            DeviceConnectStruct buffer {
                cur_dev->vendor_id,                              // Vendor ID
                cur_dev->product_id,                             // Product ID
                QString::fromWCharArray(cur_dev->serial_number), // Serial number
                QString(cur_dev->path)                           // Path
            };

            sl.push_back(buffer);
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    return sl;
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
