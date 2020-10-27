#include "eusbworker.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "eprotocom.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

#if _MSC_VER && !__INTEL_COMPILER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

void appendSize(QByteArray &ba, int size)
{
    char byte = static_cast<char>(size % 0x100);
    ba.append(byte);
    byte = static_cast<char>(size / 0x100);
    ba.append(byte);
}

EUsbWorker::EUsbWorker(DeviceConnectStruct dev, LogClass *logh, bool writelog, QObject *parent)
    : m_deviceInfo(dev), log(logh), WriteUSBLog(writelog), QObject(parent)
{
    HidDevice = nullptr;
}

EUsbWorker::~EUsbWorker()
{
}

Error::Msg EUsbWorker::setupConnection()
{
    if ((deviceInfo().vendor_id == 0) || (deviceInfo().product_id == 0))
    {
        ERMSG("DeviceInfo is null");
        Finish();
        return Error::Msg::GeneralError;
    }
#ifdef __linux__
    HidDevice = hid_open_path(deviceInfo().path.toStdString().c_str());
#endif
#ifdef _WIN32
    HidDevice = hid_open(deviceInfo().vendor_id, deviceInfo().product_id, (wchar_t *)deviceInfo().serial.utf16());
#endif
    if (!HidDevice)
    {
        qDebug() << QString::fromWCharArray(hid_error(HidDevice));
        ERMSG("Error opening HID device");
        Finish();
        return Error::Msg::GeneralError;
    }
    hid_set_nonblocking(HidDevice, 1);
    INFOMSG("HID opened successfully");
    return Error::Msg::NoError;
}

void EUsbWorker::interact()
{
    int bytes;
    unsigned char data[UH::MaxSegmenthLength + 1]; // +1 to ID

    while (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
    {
        // check if there's any data in input buffer
        if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
            continue;
        if (HidDevice != nullptr)
        {
            bytes = hid_read(HidDevice, data, UH::MaxSegmenthLength + 1);
            // Write
            if (bytes < 0)
            {
                if (WriteUSBLog)
                    log->WriteRaw("UsbThread: Unable to hid_read()");
                continue;
            }
            // Read
            if (bytes > 0)
            {
                QByteArray ba(reinterpret_cast<char *>(data), bytes);
                emit NewDataReceived(ba);
            }
            CheckWriteQueue(); // write data to port if there's something delayed in out queue
        }
    }
    Finish();
}

DeviceConnectStruct EUsbWorker::deviceInfo() const
{
    return m_deviceInfo;
}

void EUsbWorker::setDeviceInfo(DeviceConnectStruct deviceInfo)
{
    m_deviceInfo = deviceInfo;
}

int EUsbWorker::WriteDataAttempt(QByteArray &ba)
{
    WriteQueue.append(ba);
    return ba.size();
}

void EUsbWorker::closeConnection()
{
    qDebug(__PRETTY_FUNCTION__);
    if (HidDevice)
    {
        mutex_.lock();
        WriteQueue.clear();
        mutex_.unlock();
        hid_close(HidDevice);

        HidDevice = nullptr;
    }
}

void EUsbWorker::Finish()
{
    closeConnection();
    INFOMSG("UThread finished");
    emit finished();
}

Error::Msg EUsbWorker::WriteData(QByteArray &ba)
{
    if (HidDevice)
    {
        if (ba.size() > UH::MaxSegmenthLength)
        {
            if (WriteUSBLog)
                log->WriteRaw("UsbThread: WRONG SEGMENT LENGTH!\n");
            qCritical() << "Длина сегмента больше " << QString::number(UH::MaxSegmenthLength) << " байт";
            return Error::Msg::GeneralError;
        }
        if (ba.size() < UH::MaxSegmenthLength)
            ba.append(UH::MaxSegmenthLength - ba.size(), static_cast<char>(0x00));
        ba.prepend(static_cast<char>(0x00)); // inserting ID field
        if (WriteUSBLog)
        {
            QByteArray tmpba = "UsbThread: ->" + ba.toHex() + "\n";
            log->WriteRaw(tmpba);
        }
        size_t tmpt = static_cast<size_t>(ba.size());
        if (hid_write(HidDevice, reinterpret_cast<unsigned char *>(ba.data()), tmpt) != -1)
            return Error::Msg::NoError;
        else
            return Error::Msg::GeneralError;
    }
    if (log != nullptr)
        log->WriteRaw("UsbThread: null hid device");
    return Error::Msg::GeneralError;
}

void EUsbWorker::CheckWriteQueue()
{
    QMutexLocker locker(&mutex_);
    if (!WriteQueue.isEmpty())
    {
        QByteArray ba = WriteQueue.takeFirst();
        WriteData(ba);
    }
}

void EUsbWorker::checkQueue()
{
    CommandStruct inp;
    if (DataManager::deQueue(inp) == Error::Msg::NoError)
    {
        switch (inp.cmd)
        {
        default:
            initiateSend(inp);
            break;
        }
    }
}

void EUsbWorker::initiateSend(const CommandStruct &cmdStr)
{
    switch (cmdStr.cmd)
    {
    case CN::Write::Upgrade:
    {
        QByteArray ba;
        ba.append(CN::Message::Start);
        ba.append(cmdStr.cmd);
        appendSize(ba, 0);
        WriteDataAttempt(ba);
        break;
    }
    case CN::Write::Variant:
    case CN::Write::Mode:
    case CN::Write::EraseTech: // команда стирания технологического блока
    case CN::Write::BlkCmd:
    case CN::Test:
    {
        QByteArray ba;
        ba.append(CN::Message::Start);
        ba.append(cmdStr.cmd);
        appendSize(ba, 1);
        WriteDataAttempt(ba);
        break;
    }
    case CN::Write::Hardware:
    {
        QByteArray ba;
        ba.append(CN::Message::Start);
        ba.append(cmdStr.cmd);
        int size = (cmdStr.uintarg == BoardTypes::BT_BSMZ) ? WHV_SIZE_TWOBOARDS : WHV_SIZE_ONEBOARD;
        appendSize(ba, size); // BoardType(1), HiddenBlock(16)
        ba.append(cmdStr.uintarg);
        WriteDataAttempt(ba);
        break;
    }
    case CN::Write::BlkAC:
    case CN::Write::EraseCnt:
    case CN::Write::BlkTech:
    case CN::Write::BlkData:
    {
        QByteArray ba;
        ba.append(cmdStr.uintarg);
        ba.append(cmdStr.ba);
        // WRLength = InDataSize + 1;
        // SetWRSegNum();
        // WRCheckForNextSegment(true);
        break;
    }
    default:
    {
        qCritical() << QVariant::fromValue(Error::Msg::WrongCommandError).toString();
        return;
    }
    }
}

void EUsbWorker::initiateReceive(QByteArray ba)
{
    using namespace DataTypes;
    using namespace CN::Message;

    QByteArray tmps = "<-" + ba.toHex() + "\n";
    log->WriteRaw(tmps);
    if (ba.size() < 4)
    {
        qCritical("Small byte array");
        return;
    }
    switch (ba.front())
    {
    case Start:
    {
        byte cmd = ba.at(1);
        quint16 size;
        memcpy(&size, &ba.constData()[2], 2);
        /// Обработка команды и в очередь ее
        // DataManager::addSignalToOutList();
    }
    case Continue:
    {
        byte cmd = ba.at(1);
        quint16 size;
        memcpy(&size, &ba.constData()[2], 2);
        int end = ba.indexOf("0000");
        // Contains
        if (size != 768)
        {
            /// Обработка команды и в очередь ее
        }
        else
        {
            ba.remove(0, 4);
            buffer.first += size;
            buffer.second.append(ba);
        }
    }
    default:
        qCritical() << QVariant::fromValue(CN::Unknown).toString();
        return;
    }
}
