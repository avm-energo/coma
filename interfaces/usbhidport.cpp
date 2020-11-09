#include "usbhidport.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../gen/stdfunc.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMetaEnum>
#include <QThread>
#include <array>

// TODO Придумать вариант получше
#define IN 1
#define OUT 0
void appendSize(QByteArray &ba, int size)
{
    char byte = static_cast<char>(size % 0x100);
    ba.append(byte);
    byte = static_cast<char>(size / 0x100);
    ba.append(byte);
}

inline bool checkCommand(char cmd)
{
    if (cmd == -1)
    {
        qCritical() << Error::WrongCommandError;
        return false;
    }
    return true;
}

void checkForNextSegment(QByteArray &ba)
{
    if (ba.size() > 60) { }
}

UsbHidPort::UsbHidPort(const DeviceConnectStruct &dev, LogClass *logh, bool writelog, QObject *parent)
    : QObject(parent), log(logh), m_deviceInfo(dev)
{
    HidDevice = nullptr;
}

UsbHidPort::~UsbHidPort()
{
}

Error::Msg UsbHidPort::setupConnection()
{
    if ((deviceInfo().vendor_id == 0) || (deviceInfo().product_id == 0))
    {
        ERMSG("DeviceInfo is null");
        Finish();
        return Error::Msg::NoDeviceError;
    }
#ifdef __linux__
    HidDevice = hid_open_path(deviceInfo().path.toStdString().c_str());
#endif
#ifdef _WIN32
    HidDevice = hid_open(deviceInfo().vendor_id, deviceInfo().product_id, deviceInfo().serial.toStdWString().c_str());
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

void UsbHidPort::poll()
{
    int bytes;
    std::array<byte, HID::MaxSegmenthLength + 1> array; // +1 to ID
    while (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
    {
        // check if there's any data in input buffer
        if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
            continue;
        if (HidDevice)
        {
            bytes = hid_read_timeout(HidDevice, array.data(), HID::MaxSegmenthLength + 1, HID::MainLoopDelay);
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
                emit NewDataReceived(ba);
            }
            CheckWriteQueue(); // write data to port if there's something delayed in out queue
        }
    }
    Finish();
}
// TODO Проверить длину получаемой строки, если равна 4 то убрать проверки и сделать каст
void handleTime(const char *str)
{
    quint64 time = 0;
    std::copy_n(str, 4, &time);
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, time };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void handleBitString(const QByteArray &ba)
{
    // 3c21 3c00
    // a200000087000000000003010b0002000e0000006a020000320043000451383233393736020000004323330000000004785634126070e712001
}

void handleFloat(const QByteArray &ba, quint16 addr)
{
    float blk = 0.0;
    std::copy_n(ba.constData(), sizeof(float), blk);
    DataTypes::FloatStruct resp { addr, blk };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Float, resp);
}

void handleFile(const QByteArray &ba, quint16 addr)
{
    DataTypes::FileStruct resp { addr, ba };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::File, resp);
}

void handleInt(const byte num)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, num };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void handleBool(const bool status = true, int errorSize = 0, int errorCode = 0)
{
    if (status)
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, 0 };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
    }
    else
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error, 0 };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
        qCritical() << "Error size: " << errorSize << "Error code: " << errorCode;
    }
}

inline void handleCommand(const QByteArray &ba)
{
    qCritical("We should be here, something went wrong");
}

void UsbHidPort::handle(const CN::Commands cmd)
{
    // NOTE using enum after cpp20
    using namespace CN;
    using namespace DataTypes;
    uint addr = m_currentCommand.arg1.toUInt();
    switch (cmd)
    {
    case Commands::ResultOk:

        //  GVar MS GMode MS
        if (!m_buffer.second.isEmpty())
            handleInt(m_buffer.second.front());
        handleBool();
        return;

    case Commands::ResultError:

        handleBool(false, m_buffer.first, m_buffer.second.front());
        return;

    case Commands::ReadTime:

        handleTime(m_buffer.second.constData());
        return;

    case Commands::ReadBlkStartInfo:

        handleBitString(m_buffer.second);
        return;

    case Commands::ReadBlkAC:

        handleFloat(m_buffer.second, addr);
        return;

    case Commands::ReadBlkDataA:

        handleFloat(m_buffer.second, addr);
        return;

    case Commands::ReadBlkData:

        handleFloat(m_buffer.second, addr);
        return;

    case Commands::ReadBlkTech:

        handleFloat(m_buffer.second, addr);
        return;

    case Commands::ReadProgress:

        handleBitString(m_buffer.second);
        return;

    case Commands::ReadFile:

        handleFile(m_buffer.second, addr);
        return;

    default:
    {
        handleCommand(m_buffer.second);
        return;
    }
    }
}

DeviceConnectStruct UsbHidPort::deviceInfo() const
{
    return m_deviceInfo;
}

void UsbHidPort::setDeviceInfo(DeviceConnectStruct deviceInfo)
{
    m_deviceInfo = deviceInfo;
}

void UsbHidPort::WriteDataAttempt(QByteArray &ba)
{
    WriteQueue.append(ba);
}

void UsbHidPort::closeConnection()
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

void UsbHidPort::Finish()
{
    closeConnection();
    INFOMSG("UThread finished");
    emit finished();
}

void UsbHidPort::writeLog(bool in_out, QByteArray ba)
{
#ifdef HIDUSB_LOG
    QByteArray tmpba = QByteArray(metaObject()->className());
    if (in_out)
        tmpba.append(": <-");
    else
        tmpba.append(": ->");
    tmpba.append(ba).append("\n");
    log->WriteRaw(tmpba);
#endif
}

bool UsbHidPort::WriteData(QByteArray &ba)
{
    if (HidDevice)
    {
        if (ba.size() > HID::MaxSegmenthLength)
        {
            writeLog(OUT, Error::Msg::SizeError);
            qCritical() << Error::Msg::SizeError;
            return false;
        }
        if (ba.size() < HID::MaxSegmenthLength)
            ba.append(HID::MaxSegmenthLength - ba.size(), static_cast<char>(0x00));
        ba.prepend(static_cast<char>(0x00)); // inserting ID field

        writeLog(OUT, ba.toHex());

        size_t tmpt = static_cast<size_t>(ba.size());
        if (hid_write(HidDevice, reinterpret_cast<unsigned char *>(ba.data()), tmpt) != -1)
            return true;

        qCritical() << Error::Msg::WriteError;
        return false;
    }

    writeLog(OUT, Error::Msg::NoDeviceError);
    qCritical() << Error::Msg::NoDeviceError;

    return false;
}

void UsbHidPort::CheckWriteQueue()
{
    QMutexLocker locker(&mutex_);
    if (!WriteQueue.isEmpty())
    {
        QByteArray ba = WriteQueue.takeFirst();
        WriteData(ba);
    }
}

void UsbHidPort::checkQueue()
{
    CommandStruct inp;
    if (DataManager::deQueue(inp) == Error::Msg::NoError)
    {
        switch (inp.cmd)
        {
        default:
            m_currentCommand = inp;
            initiateSend(inp);
            break;
        }
    }
}

QByteArray prepareBlock(CommandStruct &cmdStr)
{
    QByteArray ba;
    ba.append(CN::Message::Start);
    ba.append(static_cast<char>((cmdStr.cmd)));
    appendSize(ba, cmdStr.ba.size() > 0 ? cmdStr.ba.size() : 1);
    ba.append(cmdStr.arg1.toUInt());
    ba.append(cmdStr.ba);
    return ba;
}

void UsbHidPort::initiateSend(const CommandStruct &cmdStr)
{
    using namespace CN;
    QByteArray ba;
    switch (m_currentCommand.cmd)
    {
    case Commands::WriteUpgrade:
    {
        ba.append(Message::Start);
        ba.append(static_cast<char>((m_currentCommand.cmd)));
        appendSize(ba, 0);
        WriteDataAttempt(ba);
        break;
    }
    case Commands::WriteVariant:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    case Commands::WriteMode:
    case Commands::EraseTech: // команда стирания технологического блока
    case Commands::WriteBlkCmd:
    case Commands::Test:
    {
        //        ba.append(CN::Message::Start);
        //        ba.append(static_cast<char>((m_currentCommand.cmd)));
        //        appendSize(ba, 1);
        ba = prepareBlock(m_currentCommand);
        // TODO Должно сработать т.к. ba.size будет равен 0
        break;
    }
    case Commands::WriteHardware:
    {
        ba.append(CN::Message::Start);
        ba.append(static_cast<char>((m_currentCommand.cmd)));
        int size = (m_currentCommand.arg1.toUInt() == BoardTypes::BT_BSMZ) ? WHV_SIZE_TWOBOARDS : WHV_SIZE_ONEBOARD;
        appendSize(ba, size); // BoardType(1), HiddenBlock(16)
        ba.append(m_currentCommand.arg1.toUInt());
        WriteDataAttempt(ba);
        break;
    }
    case Commands::WriteBlkAC:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    case Commands::EraseCnt:
    case Commands::WriteBlkTech:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    case Commands::WriteBlkData:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    default:
    {
        qCritical() << QVariant::fromValue(Error::Msg::WrongCommandError).toString();
        return;
    }
    }
    WriteDataAttempt(ba);
}

void UsbHidPort::initiateReceive(QByteArray ba)
{
    using namespace CN::Message;

    QByteArray tmps = "<-" + ba.toHex() + "\n";
    log->WriteRaw(tmps);
    if (ba.size() < 4)
    {
        qCritical() << Error::SizeError;
        return;
    }
    if (!checkCommand(QMetaEnum::fromType<CN::Commands>().value(ba.at(1))))
        return;
    byte cmd = ba.at(1);
    switch (ba.front())
    {
    case Start:
    {
        quint16 size;
        std::copy(&ba.constData()[2], &ba.constData()[4], &size);
        m_buffer.first = size;
        m_buffer.second.setRawData(&ba.constData()[4], size);
        handle(CN::Commands(cmd));
    }
    case Continue:
    {
        quint16 size;
        std::copy(&ba.constData()[2], &ba.constData()[4], &size);
        // TODO Переписать без условия
        if (size != 768)
        {
            ba.truncate(size);
            m_buffer.first += size;
            m_buffer.second.append(ba);
            handle(CN::Commands(cmd));
        }
        else
        {
            ba.remove(0, 4);
            m_buffer.first += size;
            m_buffer.second.append(ba);
        }
    }
    default:
        qCritical() << Error::WrongCommandError;
        return;
    }
}
