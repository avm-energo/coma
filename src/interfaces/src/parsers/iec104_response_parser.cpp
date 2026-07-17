#include "interfaces/parsers/iec104_response_parser.h"

#include <QDebug>

namespace Interface
{

using namespace Iec104;

Iec104ResponseParser::Iec104ResponseParser(QObject *parent)
    : BaseResponseParser(parent)
    , m_currentCommand(Command::None)
    , m_unpacker(this)
{
    connect(&m_unpacker, &ASDUUnpacker::unpacked,   //
        this, &BaseResponseParser::responseParsed); //
    m_responses.reserve(1024);
}

void Iec104ResponseParser::updateControlBlock(const SharedControlBlock &newControlBlock) noexcept
{
    m_ctrlBlock = newControlBlock;
}

bool Iec104ResponseParser::isCompleteResponse()
{
    // APCI всегда начинается с байта 0x68. Если буфер рассинхронизировался (например,
    // предыдущая итерация отрезала кадр неверного размера), первый байт перестаёт быть
    // 0x68, и второй байт больше не является настоящей длиной кадра - без этой проверки
    // splitBuffer() резал бы буфер по случайным "длинам" бесконечно, никогда не
    // восстанавливаясь, пока буфер не очистят снаружи (например, при реконнекте).
    constexpr char apciStartByte = 0x68;
    qsizetype garbageSkipped = 0;
    while (!m_responseBuffer.isEmpty() && m_responseBuffer[0] != apciStartByte)
    {
        m_responseBuffer.remove(0, 1);
        ++garbageSkipped;
    }
    if (garbageSkipped > 0)
        emit needToLog(
            QString("Response buffer desync detected, skipped %1 garbage byte(s) to resync").arg(garbageSkipped),
            Logger::Critical);

    if (std::size_t(m_responseBuffer.size()) >= apciSize)
        if (m_responseBuffer.size() >= (std::uint8_t(m_responseBuffer[1]) + 2))
            return true;
    return false;
}

Error::Msg Iec104ResponseParser::validate()
{
    return Error::NoError;
}

Error::Msg Iec104ResponseParser::validate(const QByteArray &response) noexcept
{
    try
    {
        auto parseProduct = APCI::fromByteArray(response.left(apciSize));
        // Parse APCI check
        m_currentAPCI = parseProduct;
        // Size check
        if (m_currentAPCI.m_ctrlBlock.m_format == FrameFormat::Information)
        {
            if (std::size_t(response.size()) != m_currentAPCI.m_asduSize + apciSize)
                return Error::SizeError;
        }
        return validate(); // returns no error
    }
    catch (...)
    {
        return Error::WrongFormatError;
    }
}

void Iec104ResponseParser::verify(const Iec104::ASDU &asdu) noexcept
{
    /// TODO: shitty code
    static bool confirm = false, terminate = false;
    emit needToLog(
        QString("DIAG verify: m_currentCommand=%1 cause=0x%2 confirm=%3 terminate=%4")
            .arg(static_cast<int>(m_currentCommand))
            .arg(static_cast<int>(asdu.m_cause), 2, 16, QChar('0'))
            .arg(confirm)
            .arg(terminate),
        Logger::Debug);
    switch (m_currentCommand)
    {
    case Command::RequestGroup:
    {
        switch (asdu.m_cause)
        {
        case CauseOfTransmission::ActivationConfirm:
            confirm = true;
            break;
        case CauseOfTransmission::ActivationTermination:
            terminate = true;
            break;
        default:
            break;
        }
        if (confirm && terminate)
        {
            confirm = false;
            terminate = false;
            m_currentCommand = Command::None;
            emit requestedDataReceived();
        }
        break;
    }
    default:
        /// TODO: don't ignore other commands
        break;
    }
}

void Iec104ResponseParser::splitBuffer() noexcept
{
    while (isCompleteResponse())
    {
        auto responseSize = std::uint8_t(m_responseBuffer[1]) + 2;
        m_responses.emplace_back(m_responseBuffer.left(responseSize));
        m_responseBuffer.remove(0, responseSize);
    }
}

void Iec104ResponseParser::parse()
{
    splitBuffer();
    for (const auto &response : m_responses)
    {
        emit needToLog(QString("-> %1").arg(QString(response.toHex())), Logger::Debug);
        auto validationResult = validate(response);
        if (validationResult == Error::Msg::NoError)
        {
            switch (m_currentAPCI.m_ctrlBlock.m_format)
            {
            case FrameFormat::Information:
                parseInfoFormat(response);
                break;
            case FrameFormat::Supervisory:
                parseSupervisoryFormat();
                break;
            case FrameFormat::Unnumbered:
                parseUnnumberedFormat();
                break;
            }
        }
        else
        {
            auto errStr = "Message validation fault, reason: " + QVariant::fromValue(validationResult).toString();
            emit needToLog(errStr, Logger::Critical);
        }
    }
    m_responses.clear();
}

void Iec104ResponseParser::parseInfoFormat(const QByteArray &response) noexcept
{
    ++m_ctrlBlock->m_received;
    emit needToCheckControlBlock();
    auto asdu = ASDU::fromByteArray(response.mid(apciSize, m_currentAPCI.m_asduSize));
    if (isFileTransferType(asdu.m_msgType))
        handleFileTransferAsdu(asdu);
    else
    {
        m_unpacker.unpack(asdu);
        verify(asdu);
    }
}

bool Iec104ResponseParser::isFileTransferType(const MessageDataType type) noexcept
{
    switch (type)
    {
    case MessageDataType::F_FR_NA_1:
    case MessageDataType::F_SR_NA_1:
    case MessageDataType::F_SG_NA_1:
    case MessageDataType::F_LS_NA_1:
    case MessageDataType::F_SC_NA_1:
    case MessageDataType::F_AF_NA_1:
        return true;
    default:
        return false;
    }
}

void Iec104ResponseParser::handleFileTransferAsdu(const ASDU &asdu) noexcept
{
    // Формат m_data (после 6-байтного заголовка ASDU): [обj.адрес x3][filenum][0x00][...],
    // см. Iec104RequestParser::buildFileFrame и легаси IEC104Parser::ASDUFilePrefix.
    if (asdu.m_data.size() < 4)
        return;
    const auto fileNum = std::uint8_t(asdu.m_data[3]);
    switch (asdu.m_msgType)
    {
    case MessageDataType::F_FR_NA_1: // File ready - файл готов, в ответе передан его размер
    {
        if (asdu.m_data.size() < 8)
            return;
        m_longDataBuffer.clear();
        const quint32 fileSize = std::uint8_t(asdu.m_data[5]) //
            | (std::uint8_t(asdu.m_data[6]) << 8)             //
            | (std::uint8_t(asdu.m_data[7]) << 16);
        emit responseParsed(DataTypes::GeneralResponseStruct { DataTypes::GeneralResponseTypes::DataSize, fileSize });
        emit fileReplyNeeded(FileReplyAction::CallFile, fileNum, 0);
        break;
    }
    case MessageDataType::F_SR_NA_1: // Section ready - секция готова
    {
        if (asdu.m_data.size() < 6)
            return;
        // Номер секции всегда берём из текущего сообщения устройства, а не храним свой
        // счётчик - иначе при "двойном" F_LS (см. ниже) ответ будет ссылаться на устаревшую секцию.
        const auto section = std::uint8_t(asdu.m_data[5]);
        emit fileReplyNeeded(FileReplyAction::CallSection, fileNum, section);
        break;
    }
    case MessageDataType::F_SG_NA_1: // Segment - сегмент данных секции
    {
        if (asdu.m_data.size() < 7)
            return;
        const auto segSize = std::uint8_t(asdu.m_data[6]);
        if (segSize && asdu.m_data.size() >= (7 + segSize))
        {
            m_longDataBuffer.append(asdu.m_data.mid(7, segSize));
            emit responseParsed(DataTypes::GeneralResponseStruct { //
                DataTypes::GeneralResponseTypes::DataCount, static_cast<quint64>(m_longDataBuffer.size()) });
        }
        break;
    }
    case MessageDataType::F_LS_NA_1: // Last section/segment - конец секции или файла
    {
        if (asdu.m_data.size() < 7)
            return;
        const auto section = std::uint8_t(asdu.m_data[5]);
        const auto qualifier = std::uint8_t(asdu.m_data[6]);
        if (qualifier == 0x03) // конец не последней секции - подтверждаем именно эту секцию
            emit fileReplyNeeded(FileReplyAction::ConfirmSection, fileNum, section);
        else if (qualifier == 0x01) // конец последней секции - файл получен целиком
        {
            emit fileReplyNeeded(FileReplyAction::ConfirmFile, fileNum, section);
            fileReceived(m_longDataBuffer, S2::FilesEnum(fileNum), m_request.arg2.value<DataTypes::FileFormat>());
            m_longDataBuffer.clear();
            emit requestedDataReceived();
        }
        break;
    }
    case MessageDataType::F_SC_NA_1: // Call - запрос от устройства при записи файла
    {
        if (asdu.m_data.size() < 7)
            return;
        const auto qualifier = std::uint8_t(asdu.m_data[6]);
        const auto payload = m_request.arg2.value<QByteArray>();
        if (qualifier == 0x02) // запрос файла (размера)
            emit fileWriteReplyNeeded(FileWriteReplyAction::SectionReady, payload);
        else if (qualifier == 0x06) // запрос сегментов
            emit fileWriteReplyNeeded(FileWriteReplyAction::SendSegments, payload);
        break;
    }
    case MessageDataType::F_AF_NA_1: // Ack - подтверждение от устройства при записи файла
    {
        if (asdu.m_data.size() < 7)
            return;
        const auto qualifier = std::uint8_t(asdu.m_data[6]);
        if (qualifier == 0x03) // секция принята - завершаем файл
            emit fileWriteReplyNeeded(FileWriteReplyAction::FileFinal, m_request.arg2.value<QByteArray>());
        else if (qualifier == 0x01) // файл принят целиком - готово
        {
            const auto size = static_cast<quint64>(m_request.arg2.value<QByteArray>().size());
            emit responseParsed(DataTypes::GeneralResponseStruct { DataTypes::GeneralResponseTypes::Ok, size });
            emit requestedDataReceived();
            emit writeCompleted();
        }
        break;
    }
    default:
        break;
    }
}

void Iec104ResponseParser::parseSupervisoryFormat() noexcept
{
    /// Need action?
}

void Iec104ResponseParser::parseUnnumberedFormat() noexcept
{
    /// TODO: Send data to request parser?
    emit unnumberedFormatReceived(m_currentAPCI.m_ctrlBlock.m_func, m_currentAPCI.m_ctrlBlock.m_arg);
}

void Iec104ResponseParser::receiveCurrentCommand(const Iec104::Command currCommand) noexcept
{
    emit needToLog(
        QString("DIAG receiveCurrentCommand: %1 -> %2")
            .arg(static_cast<int>(m_currentCommand))
            .arg(static_cast<int>(currCommand)),
        Logger::Debug);
    m_currentCommand = currCommand;
}

} // namespace Interface
