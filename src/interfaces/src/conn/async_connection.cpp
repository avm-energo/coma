#include "interfaces/conn/async_connection.h"

#include <gen/std_ext.h>
#include <s2/s2util.h>

namespace Interface
{

AsyncConnection::AsyncConnection(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<State>();
    qRegisterMetaType<DeviceResponse>();
    qRegisterMetaType<ProtocolDescription>();
}

RequestQueue &AsyncConnection::getQueue() noexcept
{
    return m_queue;
}

void AsyncConnection::updateProtocol(const ProtocolDescription &desc) noexcept
{
    emit protocolSettingsUpdated(desc);
}

void AsyncConnection::reqAlarms(quint32 addr, quint32 count)
{
    setToQueue(CommandStruct { Commands::C_ReqAlarms, addr, count });
}

void AsyncConnection::reqFloats(quint32 addr, quint32 count)
{
    setToQueue(CommandStruct { Commands::C_ReqFloats, addr, count });
}

void AsyncConnection::reqBitStrings(quint32 addr, quint32 count)
{
    if (addr == addr::bsiStartReg)
        reqBSI();
    else if (addr == addr::bsiExtStartReg)
        reqBSIExt();
    else
        setToQueue(CommandStruct { Commands::C_ReqBitStrings, addr, count });
}

void AsyncConnection::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    setToQueue(CommandStruct { Commands::C_ReqStartup, sigAdr, sigCount });
}

void AsyncConnection::reqBSI()
{
    setToQueue(CommandStruct { Commands::C_ReqBSI, addr::bsiStartReg, addr::bsiCountRegs });
}

void AsyncConnection::reqBSIExt()
{
    setToQueue(CommandStruct { Commands::C_ReqBSIExt, addr::bsiExtStartReg, addr::bsiExtCountRegs });
}

void AsyncConnection::reqFile(quint32 id, FileFormat format, quint32 expectedSize)
{
    if (expectedSize != 0)
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, expectedSize };
        emit response(resp);
    }
    setToQueue(CommandStruct { Commands::C_ReqFile, id, format });
}

void AsyncConnection::writeFile(quint32 id, const QByteArray &ba)
{
    setToQueue(CommandStruct { Commands::C_WriteFile, id, ba });
}

void AsyncConnection::writeConfiguration(const QByteArray &ba)
{
    constexpr auto fileType = std_ext::to_underlying(S2::FilesEnum::Config);
    writeFile(fileType, ba);
    emit silentReconnectMode();
}

void AsyncConnection::writeFirmware(const QByteArray &ba)
{
    constexpr auto fileType = std_ext::to_underlying(S2::FilesEnum::Firmware);
    writeFile(fileType, ba);
}

void AsyncConnection::reqTime()
{
    setToQueue(CommandStruct { Commands::C_ReqTime, addr::timeReg, 1 });
}

void AsyncConnection::writeTime(quint32 time)
{
    setToQueue(CommandStruct { Commands::C_WriteTime, time, 0 });
}

#ifdef Q_OS_LINUX
void AsyncConnection::writeTime(const timespec &time)
{
    setToQueue(CommandStruct { Commands::C_WriteTime, QVariant::fromValue(time), QVariant() });
}
#endif

void AsyncConnection::writeCommand(Commands cmd, QVariant value)
{
    setToQueue(CommandStruct { cmd, value, QVariant() });
    if (cmd == Commands::C_StartFirmwareUpgrade)
        emit silentReconnectMode();
}

void AsyncConnection::writeCommand(Commands cmd, const QVariantList &list)
{
    setToQueue(CommandStruct { cmd, list, QVariant() });
}

void AsyncConnection::responseHandle(const Interface::DeviceResponse &resp)
{
    std::visit([this](auto &&var) { emit response(var); }, resp);
    emit queueSizeChanged(m_queue.size());
}

void AsyncConnection::setToQueue(CommandStruct &&cmd)
{
    m_queue.addToQueue(std::move(cmd));
    emit queueSizeChanged(m_queue.size());
}

} // namespace Interface
