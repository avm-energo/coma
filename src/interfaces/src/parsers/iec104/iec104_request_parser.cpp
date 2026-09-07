#include "interfaces/parsers/iec104/iec104_request_parser.h"

#include <libavm-gen/datatypes.h>
#include <libavm-gen/std_ext.h>
#include <libavm-gen/stdfunc.h>
#include <s2/filestruct.h>

namespace Interface
{

using namespace Iec104;

Iec104RequestParser::Iec104RequestParser(QObject *parent) : BaseRequestParser(parent), m_baseStationAddress(0) { }

void Iec104RequestParser::basicProtocolSetup() noexcept
{
    using namespace Protocol;
    m_protocol.addGroup(Iec104Group { 1, 15, 0, 1 }); // BSI request
    /// TODO: добавить загрузку ВПО, секретные операции
}

Protocol::Iec104Group Iec104RequestParser::getGroupByAddress(const quint32 addr) const noexcept
{
    return getGroupsByAddress<Protocol::Iec104Group>(addr);
}

void Iec104RequestParser::setBaseStationAddress(const quint16 bsAddress) noexcept
{
    m_baseStationAddress = bsAddress;
}

void Iec104RequestParser::updateControlBlock(const SharedControlBlock &newControlBlock) noexcept
{
    m_ctrlBlock = newControlBlock;
}

QByteArray Iec104RequestParser::parse(const CommandStruct &cmd)
{
    m_request.clear();
    switch (cmd.command)
    {
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    case Commands::C_ReqStartup:
    case Commands::C_ReqAlarms:
    case Commands::C_ReqFloats:
    case Commands::C_ReqBitStrings:
    case Commands::C_ReqTime:
    {
        const auto addr = cmd.arg1.value<quint16>();
        const auto count = cmd.arg2.value<quint16>();
        const auto group = getGroupByAddress(addr);
        if (group.m_startAddr == addr && group.m_count == count)
        {
            switch (static_cast<CauseOfTransmission>(group.m_transType))
            {
            case CauseOfTransmission::GroupRequest:
                m_request = createGroupRequest(group.m_group);
                break;
            case CauseOfTransmission::Periodic:
            case CauseOfTransmission::Spontaneous:
                m_isExceptionalSituation = true; // there's nothing to do about
                break;
            default:
                // ignore other causes
                break;
            }
        }
        break;
    }
    case Commands::C_GI:
    {
        m_request = createGroupRequest(0x00);
        break;
    }
    case Commands::C_WriteTime:
    {
        // Commands104::CommandStruct inp { Commands104::CM104_COM51, time, 0, {} };
        break;
    }
    case Commands::C_ReqFile:
    {
        const auto id = cmd.arg1.value<quint32>();
        if (id > std::numeric_limits<quint8>::max())
        {
            qDebug() << "IEC104: file id" << id << "is out of range for file transfer";
            break;
        }
        m_request = createFileReply(Iec104::FileReplyAction::Select, static_cast<quint8>(id), 0);
        break;
    }
    case Commands::C_WriteFile:
    {
        const auto id = cmd.arg1.value<quint32>();
        if (id > std::numeric_limits<quint8>::max() || !cmd.arg2.canConvert<QByteArray>())
        {
            qDebug() << "IEC104: invalid write file request, id" << id;
            break;
        }
        const auto file = cmd.arg2.value<QByteArray>();
        m_request = createFileReadyMessage(static_cast<quint8>(id), static_cast<quint32>(file.size()));
        break;
    }
    case Commands::C_StartFirmwareUpgrade:
    {
        m_request = createSingleCommand(FirmwareUpgrade, true);
        break;
    }
    case Commands::C_WriteUserValues:
    {
        // Q_ASSERT(item.canConvert<DataTypes::FloatStruct>());
        // if (!item.canConvert<DataTypes::FloatStruct>())
        //     return;
        // DataTypes::FloatStruct flstr = item.value<DataTypes::FloatStruct>();
        // Commands104::CommandStruct inp { Commands104::CM104_COM50, flstr.sigAdr, flstr.sigVal, {} };
        break;
    }
    // "WS" commands
    case Commands::C_StartWorkingChannel:
    case Commands::C_EraseJournals:
    case Commands::C_SetStartupValues:
    case Commands::C_SetStartupPhaseA:
    case Commands::C_SetStartupPhaseB:
    case Commands::C_SetStartupPhaseC:
    case Commands::C_ClearStartupUnbounced:
    case Commands::C_SetTransOff:
    case Commands::C_ClearStartupError:
    case Commands::C_ClearStartupValues:
    case Commands::C_SetStartupUnbounced:
    {
        auto search = s_wsCmdMap.find(cmd.command);
        if (search != s_wsCmdMap.cend())
        {
            [[maybe_unused]] auto value = search->second; // auto iec104cmd = get104Command(cmd);
            // Commands104::CommandStruct inp { Commands104::CM104_COM45, iec104cmd, 0, true, {} };
        }
        break;
    }
    default:
        qDebug() << "Undefined command: " << cmd.command;
    }

    return m_request;
}

QByteArray Iec104RequestParser::getNextContinueCommand() noexcept
{
    return QByteArray { };
}

void Iec104RequestParser::exceptionalAction(const CommandStruct &command) noexcept
{
    Q_UNUSED(command);
    m_isExceptionalSituation = false;
}

QByteArray Iec104RequestParser::createSingleCommand(const quint32 address, const bool value)
{
    try
    {
        ASDU asdu(m_baseStationAddress);
        asdu.setRequestData(uint24(address), value);
        auto request = asdu.toByteArray();
        APCI apci(*m_ctrlBlock, request.size());
        apci.m_ctrlBlock.m_format = FrameFormat::Information;
        request.prepend(apci.toByteArray());
        emit currentCommand(Iec104::Command::Command45);
        return request;
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        return QByteArray();
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        return QByteArray();
    }
}

QByteArray Iec104RequestParser::createGroupRequest(const quint32 groupNum)
{
    try
    {
        ASDU asdu(m_baseStationAddress);
        asdu.setRequestData(groupNum);
        auto request = asdu.toByteArray();
        APCI apci(*m_ctrlBlock, request.size());
        apci.m_ctrlBlock.m_format = FrameFormat::Information;
        request.prepend(apci.toByteArray());
        emit currentCommand(Iec104::Command::RequestGroup);
        return request;
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        return QByteArray();
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        return QByteArray();
    }
}

QByteArray Iec104RequestParser::buildFileFrame(
    const Iec104::MessageDataType type, const quint8 fileNum, const quint8 section, const quint8 qualifier)
{
    return buildFileFrame(type, fileNum, section, QByteArray(1, static_cast<char>(qualifier)));
}

QByteArray Iec104RequestParser::buildFileFrame(
    const Iec104::MessageDataType type, const quint8 fileNum, const quint8 section, const QByteArray &tail)
{
    try
    {
        ASDU asdu(m_baseStationAddress);
        asdu.setFileTransferData(type, fileNum, section, tail);
        auto request = asdu.toByteArray();
        APCI apci(*m_ctrlBlock, request.size());
        apci.m_ctrlBlock.m_format = FrameFormat::Information;
        request.prepend(apci.toByteArray());
        return request;
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        return QByteArray();
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        return QByteArray();
    }
}

QByteArray Iec104RequestParser::createFileReply(
    const Iec104::FileReplyAction action, const quint8 fileNum, const quint8 section) noexcept
{
    switch (action)
    {
    case Iec104::FileReplyAction::Select:
        return buildFileFrame(
            MessageDataType::F_SC_NA_1, fileNum, 0, std_ext::to_underlying(SelectAndCallQualifier::SelectFile));
    case Iec104::FileReplyAction::CallFile:
        return buildFileFrame(
            MessageDataType::F_SC_NA_1, fileNum, 0, std_ext::to_underlying(SelectAndCallQualifier::RequestFile));
    case Iec104::FileReplyAction::CallSection:
        return buildFileFrame(MessageDataType::F_SC_NA_1, fileNum, section,
            std_ext::to_underlying(SelectAndCallQualifier::RequestSection));
    case Iec104::FileReplyAction::ConfirmSection:
        return buildFileFrame(
            MessageDataType::F_AF_NA_1, fileNum, section, std_ext::to_underlying(AckFileQualifier::PositiveSection));
    case Iec104::FileReplyAction::ConfirmFile:
        return buildFileFrame(
            MessageDataType::F_AF_NA_1, fileNum, section, std_ext::to_underlying(AckFileQualifier::PositiveFile));
    }
    return QByteArray();
}

QByteArray Iec104RequestParser::createFileReadyMessage(const quint8 fileNum, const quint32 fileSize) noexcept
{
    // Секция "съедается" первым байтом размера файла - см. легаси IEC104Parser::fileReady (cmd.chop(1)).
    QByteArray tail;
    tail.append(static_cast<char>((fileSize >> 8) & 0xFF));
    tail.append(static_cast<char>((fileSize >> 16) & 0xFF));
    return buildFileFrame(MessageDataType::F_FR_NA_1, fileNum, static_cast<quint8>(fileSize & 0xFF), tail);
}

QByteArray Iec104RequestParser::createSectionReadyMessage(
    const quint8 fileNum, const quint8 section, const quint32 sectionSize) noexcept
{
    // fileNum - реальный номер файла (тот же, что в createFileReadyMessage), не захардкоженная
    // "легаси" единица - иначе устройство отвечает SCQ/AFQ с ошибкой "несуществующее имя файла"
    // (ГОСТ 101 §7.2.6.30/32), т.к. ждёт продолжения именно ТОГО файла, что было объявлено.
    QByteArray tail;
    tail.append(static_cast<char>(sectionSize & 0xFF));
    tail.append(static_cast<char>((sectionSize >> 8) & 0xFF));
    tail.append(static_cast<char>((sectionSize >> 16) & 0xFF));
    return buildFileFrame(MessageDataType::F_SR_NA_1, fileNum, section, tail);
}

QList<QByteArray> Iec104RequestParser::splitIntoSegments(const QByteArray &payload) const noexcept
{
    constexpr int segmentSize = 230;
    QList<QByteArray> chunks;
    for (int pos = 0; pos < payload.size(); pos += segmentSize)
        chunks.append(payload.mid(pos, segmentSize));
    return chunks;
}

QByteArray Iec104RequestParser::buildSegmentFrame(
    const quint8 fileNum, const quint8 section, const QByteArray &chunk) noexcept
{
    QByteArray tail(1, static_cast<char>(chunk.size()));
    tail.append(chunk);
    return buildFileFrame(MessageDataType::F_SG_NA_1, fileNum, section, tail);
}

QByteArray Iec104RequestParser::buildLastSectionFrame(
    const quint8 fileNum, const quint8 section, const QByteArray &sectionPayload) noexcept
{
    quint8 checksum = 0;
    for (const auto byte : sectionPayload)
        checksum += static_cast<quint8>(byte);
    QByteArray lsTail(
        1, static_cast<char>(std_ext::to_underlying(LastSectionQualifier::SectionTransferNoDeactivation)));
    lsTail.append(static_cast<char>(checksum));
    return buildFileFrame(MessageDataType::F_LS_NA_1, fileNum, section, lsTail);
}

QByteArray Iec104RequestParser::createFileFinalMessage(
    const quint8 fileNum, const quint8 section, const QByteArray &fullFilePayload) noexcept
{
    quint8 checksum = 0;
    for (const auto byte : fullFilePayload)
        checksum += static_cast<quint8>(byte);
    QByteArray tail(1, static_cast<char>(std_ext::to_underlying(LastSectionQualifier::FileTransferNoDeactivation)));
    tail.append(static_cast<char>(checksum));
    return buildFileFrame(MessageDataType::F_LS_NA_1, fileNum, section, tail);
}

QByteArray Iec104RequestParser::createStartMessage() const noexcept
{
    try
    {
        APCI apci;
        apci.m_ctrlBlock.m_format = FrameFormat::Unnumbered;
        apci.m_ctrlBlock.m_func = ControlFunc::StartDataTransfer;
        apci.m_ctrlBlock.m_arg = ControlArg::Activate;
        auto bytes = apci.toByteArray();
        return bytes;
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        return QByteArray();
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        return QByteArray();
    }
}

QByteArray Iec104RequestParser::createStopMessage() const noexcept
{
    try
    {
        APCI apci;
        apci.m_ctrlBlock.m_format = FrameFormat::Unnumbered;
        apci.m_ctrlBlock.m_func = ControlFunc::StopDataTransfer;
        apci.m_ctrlBlock.m_arg = ControlArg::Activate;
        auto bytes = apci.toByteArray();
        return bytes;
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        return QByteArray();
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        return QByteArray();
    }
}

QByteArray Iec104RequestParser::createTestMessage(Iec104::ControlArg arg) const noexcept
{
    try
    {
        APCI apci;
        apci.m_ctrlBlock.m_format = FrameFormat::Unnumbered;
        apci.m_ctrlBlock.m_func = ControlFunc::TestFrame;
        apci.m_ctrlBlock.m_arg = arg;
        auto bytes = apci.toByteArray();
        return bytes;
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        return QByteArray();
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        return QByteArray();
    }
}

QByteArray Iec104RequestParser::createSupervisoryMessage() const noexcept
{
    try
    {
        APCI apci(*m_ctrlBlock);
        apci.m_ctrlBlock.m_format = FrameFormat::Supervisory;
        auto bytes = apci.toByteArray();
        return bytes;
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        return QByteArray();
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        return QByteArray();
    }
}

} // namespace Interface
