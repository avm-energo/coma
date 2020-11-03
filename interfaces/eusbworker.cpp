#include "eusbworker.h"

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

EUsbWorker::EUsbWorker(const DeviceConnectStruct &dev, LogClass *logh, bool writelog, QObject *parent)
    : QObject(parent), log(logh), WriteUSBLog(writelog), m_deviceInfo(dev)
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
                initiateReceive(ba);
            }
            CheckWriteQueue(); // write data to port if there's something delayed in out queue
        }
    }
    Finish();
}
// TODO Проверить длину получаемой строки, если равна 4 то убрать проверки и сделать каст
inline void handleTime(const char *str)
{
    quint64 time;
    std::copy_n(str, 4, &time);
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, time };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

inline void handleBitString(const QByteArray &ba)
{
    // 3c21 3c00
    // a200000087000000000003010b0002000e0000006a020000320043000451383233393736020000004323330000000004785634126070e712001
}
inline void handleFloat(const QByteArray &ba, quint16 addr)
{
    float blk = 0.0;
    std::copy_n(ba.constData(), sizeof(float), blk);
    DataTypes::FloatStruct resp { addr, blk };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Float, resp);
}
inline void handleFile(const QByteArray &ba, quint16 addr)
{
    DataTypes::FileStruct resp { addr, ba };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::File, resp);
}
inline void handleCommand(const QByteArray &ba)
{
    qCritical("We should be here, something went wrong");
}

void EUsbWorker::handle(const CN::Commands cmd)
{
    using namespace CN;
    using namespace DataTypes;
    uint addr = m_currentCommand.arg1.toUInt();
    // using enum after cpp20
    switch (cmd)
    {
    case Commands::ResultOk:

        // TODO не забыть про  GVar MS c 0 0    SS OK 1 0 V и  GMode MS c 0 0   SS OK 1 0 V
        qInfo();
        return;

    case Commands::ResultError:

        qCritical() << "Error size: " << m_buffer.first << "Error code: " << m_buffer.second.front();
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
            return Error::Msg::SizeError;
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
            return Error::Msg::WriteError;
    }
    if (log != nullptr)
        log->WriteRaw("UsbThread: null hid device");
    return Error::Msg::NoDeviceError;
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

void EUsbWorker::initiateSend(const CommandStruct &cmdStr)
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

void EUsbWorker::initiateReceive(QByteArray ba)
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
