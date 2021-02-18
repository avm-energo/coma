#include "usbhidport.h"

#ifdef QT_GUI_LIB
#include "../widgets/coma.h"
#include "../widgets/wd_func.h"
#endif

#include "../gen/helper.h"
#include "../gen/stdfunc.h"
#include "baseinterface.h"

//#ifdef QT_GUI_LIB
//#include <QApplication>
//#endif
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMetaEnum>
#include <QRegularExpression>
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
    m_shouldBeStopped = false;
    connect(this, &UsbHidPort::clearQueries, &UsbHidPort::clear);
}

UsbHidPort::~UsbHidPort()
{
    // qDebug() << "UsbHidPort deleted";
}
#ifdef QT_GUI_LIB
void UsbHidPort::connectToGui(QObject *object)
{
    // Q_ASSERT(object != nullptr);
    // Q_ASSERT(object->parent() != nullptr);
    Coma *mainWindow = qobject_cast<Coma *>(WDFunc::getMainWindow());
    //  Coma *mainWindow = qobject_cast<Coma *>(object->parent());
    Q_ASSERT(mainWindow != nullptr);
    if (mainWindow != nullptr)
        connect(mainWindow, &Coma::sendMessage, this, &UsbHidPort::nativeEvent, Qt::DirectConnection);
}
#endif
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
    //#ifdef __linux__
    //   m_hidDevice = hid_open_path(deviceInfo().path.toStdString().c_str());
    //#endif
    //#ifdef _WIN32
    m_hidDevice = openDevice(m_deviceInfo);
    //#endif
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
    // while (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
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
        if (iface->state() == BaseInterface::State::Wait)
        // if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
        {
            StdFunc::Wait(500);
            continue;
        }
        if (!m_hidDevice)
            continue;
        std::array<byte, HID::MaxSegmenthLength + 1> array; // +1 to ID
        bytes = hid_read_timeout(m_hidDevice, array.data(), HID::MaxSegmenthLength + 1, 30);
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
    BaseInterface::iface()->setState(BaseInterface::State::Run);
    // Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    qDebug() << timer.elapsed();
}

void UsbHidPort::deviceDisconnected(const UsbHidSettings &st)
{
    // Отключено другое устройство
    if (st != deviceInfo())
        return;
    // Отключено наше устройство
    BaseInterface::iface()->setState(BaseInterface::State::Wait);
    // Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
    qInfo() << deviceInfo() << "disconnected";
}

void UsbHidPort::deviceConnected()
{
    if (!setupConnection())
        return;
    qInfo() << deviceInfo() << "connected";
    BaseInterface::iface()->setState(BaseInterface::State::Run);
    // Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
}

void UsbHidPort::deviceDisconnected()
{
    // Отключено наше устройство
    BaseInterface::iface()->setState(BaseInterface::State::Wait);
    // Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
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

void UsbHidPort::nativeEvent(void *message)
{
#ifdef _WIN32
    MSG *msg = static_cast<MSG *>(message);
    int msgType = msg->message;
    Q_ASSERT(msgType == WM_DEVICECHANGE);

    DEV_BROADCAST_DEVICEINTERFACE *devint = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(msg->lParam);

    switch (msg->wParam)
    {

    case DBT_DEVICEARRIVAL:
    {
        if (devint->dbcc_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
            break;
        QString str = QString::fromStdWString(&devint->dbcc_name[0]);
        QRegularExpression regex(HID::headerValidator);
        QRegularExpressionMatch match = regex.match(str);
        if (!match.hasMatch())
            break;
        if (match.captured(0) != "USB" && match.captured(0) != "HID")
            break;
        qDebug() << str << /*st*/ /*<<*/ devint->dbcc_devicetype;
        if (deviceInfo().hasMatch(str))
        {
            if (!shouldBeStopped())
                deviceConnected();
            shouldBeStopped(false);
            break;
        }
        if (deviceInfo().hasPartialMatch(str))
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
        if (devint->dbcc_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
            return;
        QString str = QString::fromStdWString(&devint->dbcc_name[0]);
        QRegularExpression regex(HID::headerValidator);
        QRegularExpressionMatch match = regex.match(str);
        if (!match.hasMatch())
            break;
        if (match.captured(0) != "USB" && match.captured(0) != "HID")
            break;
        qDebug() << str << /*st*/ /*<<*/ devint->dbcc_devicetype;
        if (deviceInfo().hasMatch(str))
        {
            if (shouldBeStopped())
                deviceDisconnected();
            shouldBeStopped(true);
            break;
        }
        if (deviceInfo().hasPartialMatch(str))
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
        qInfo() << "Unhadled case" << QString::number(msg->wParam, 16);
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
    return true;
}

void UsbHidPort::checkQueue()
{
    QMutexLocker locker(&_mutex);
    if (m_writeQueue.isEmpty())
        return;
    QByteArray ba = m_writeQueue.takeFirst();
    locker.unlock();
    if (writeData(ba))
        m_waitForReply = true;
    else
        emit clearQueries();
}
