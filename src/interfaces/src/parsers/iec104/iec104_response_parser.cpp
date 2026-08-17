#include "interfaces/parsers/iec104/iec104_response_parser.h"

#include <QDebug>

#include <algorithm>

namespace Interface
{

using namespace Iec104;

/// \brief Максимальный размер одной секции файла при записи на устройство.
/// \see Av-tuk.core: M52G/AV-TUK_conf/conf104.h - #define SECTIONSIZE 2048.
constexpr quint32 writeSectionSize = 2048;

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
    switch (m_currentCommand)
    {
    case Command::Command45: // одиночная команда (C_SC_NA_1) - устройство шлёт ACTCON, см. WRITE_COM45 в Write104.c
    {
        // Единственный потребитель Command45 сейчас - C_StartFirmwareUpgrade (адрес 802). Устройство
        // шлёт ACTCON СИНХРОННО, ДО вызова обработчика команды (см. Write104.c: WriteCom45(...,ACTCON,...)
        // идёт раньше Func45(...)), а сам обработчик (Com104Handler, case 802) сразу запускает
        // ResetSystem() - т.е. реальный ребут устройства. ACTTERM формально шлётся следом, но по факту
        // (см. живой pcap) не успевает уйти до разрыва TCP-соединения - устройство просто рвёт связь
        // (RST) вместо него. Поэтому, в отличие от RequestGroup, ACTCON здесь - самодостаточный признак
        // успеха; ждать ACTTERM для этой команды означает ждать вечно и никогда не показать пользователю
        // попап об успешном переходе на новое ПО.
        if (asdu.m_cause == CauseOfTransmission::ActivationConfirm)
        {
            m_actConfirmed = false;
            m_actTerminated = false;
            m_currentCommand = Command::None;
            processOk();
            emit requestedDataReceived();
        }
        break;
    }
    case Command::RequestGroup:
    {
        switch (asdu.m_cause)
        {
        case CauseOfTransmission::ActivationConfirm:
            m_actConfirmed = true;
            break;
        case CauseOfTransmission::ActivationTermination:
            m_actTerminated = true;
            break;
        default:
            break;
        }
        if (m_actConfirmed && m_actTerminated)
        {
            m_actConfirmed = false;
            m_actTerminated = false;
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
                emit infoTransferFormatReceived(m_currentAPCI.m_ctrlBlock.m_counters);
                parseInfoFormat(response);
                break;
            case FrameFormat::Supervisory:
                emit SupervisoryFormatReceived(m_currentAPCI.m_ctrlBlock.m_counters);
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

QByteArray Iec104ResponseParser::writeSectionSlice(const QByteArray &payload) const noexcept
{
    return payload.mid((m_currentWriteSection - 1) * writeSectionSize, writeSectionSize);
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
        // SCQ (Указатель выбора и вызова, ГОСТ 101 §7.2.6.30) = CP8{select[1..4], qualifier[5..8]} -
        // младший нибл несёт код действия ("select"), старший - НЕЗАВИСИМЫЙ код ошибки (1..5:
        // область недоступна/ошибка CRC/недопустимая услуга/несуществующее имя файла/секции). При
        // одной секции старший нибл был всегда 0, поэтому точное сравнение байта целиком "случайно"
        // работало - начиная со второй секции там появляется код ошибки (например, 0x46 = select
        // "запрос секции" + qualifier "несуществующее имя файла"), и точное сравнение теряло
        // сообщение. Сверяем только select; сам код ошибки в старшем нибле пока не обрабатываем.
        const auto select = std::uint8_t(asdu.m_data[6]) & 0x0F;
        const auto fileNum = static_cast<quint8>(m_request.arg1.value<quint32>());
        const auto payload = m_request.arg2.value<QByteArray>();
        if (select == 0x02) // запрос файла - начинаем с первой секции
        {
            m_currentWriteSection = 1;
            // Реактивный протокол записи файла не проходит через общий механизм
            // getNextDataSection/totalWritingBytes (он рассчитан на клиент-ведущую отправку кусками
            // у Modbus/Protocom) - progress bar для IEC104 ведём вручную по ходу секций.
            emit responseParsed(
                DataTypes::GeneralResponseStruct { DataTypes::GeneralResponseTypes::DataSize, quint64(payload.size()) });
            emit fileWriteReplyNeeded(
                FileWriteReplyAction::SectionReady, fileNum, m_currentWriteSection, writeSectionSlice(payload));
        }
        else if (select == 0x06) // запрос сегментов ТЕКУЩЕЙ секции
            emit fileWriteReplyNeeded(
                FileWriteReplyAction::SendSegments, fileNum, m_currentWriteSection, writeSectionSlice(payload));
        break;
    }
    case MessageDataType::F_AF_NA_1: // Ack - подтверждение от устройства при записи файла
    {
        if (asdu.m_data.size() < 7)
            return;
        // AFQ (ГОСТ 101 §7.2.6.32) той же структуры, что SCQ (см. комментарий выше) - сверяем
        // только младший нибл (код подтверждения), не байт целиком.
        const auto qualifier = std::uint8_t(asdu.m_data[6]) & 0x0F;
        const auto fileNum = static_cast<quint8>(m_request.arg1.value<quint32>());
        const auto payload = m_request.arg2.value<QByteArray>();
        if (qualifier == 0x03) // секция принята
        {
            // Байт, реально принятых устройством к этому моменту (текущая секция уже подтверждена).
            const auto bytesWritten = std::min(quint32(m_currentWriteSection) * writeSectionSize, quint32(payload.size()));
            emit responseParsed(DataTypes::GeneralResponseStruct { DataTypes::GeneralResponseTypes::DataCount, bytesWritten });
            // Есть ли ещё данные за пределами уже отправленных секций - если да, переходим
            // к следующей секции, если нет - шлём финальный F_LS (qualifier=1), секций больше нет.
            const bool hasMoreSections = quint32(m_currentWriteSection) * writeSectionSize < quint32(payload.size());
            if (hasMoreSections)
            {
                ++m_currentWriteSection;
                emit fileWriteReplyNeeded(
                    FileWriteReplyAction::SectionReady, fileNum, m_currentWriteSection, writeSectionSlice(payload));
            }
            else
                // Контрольная сумма финального F_LS (qualifier=1) считается устройством НЕ по последней
                // секции, а нарастающим итогом по ВСЕМУ файлу (KS_FileGet в File104.c, обнуляется только
                // при приёме F_FR_NA_1 и копится на каждом сегменте каждой секции) - поэтому сюда, в отличие
                // от остальных действий, передаём payload целиком, а не срез текущей секции.
                emit fileWriteReplyNeeded(FileWriteReplyAction::FileFinal, fileNum, m_currentWriteSection, payload);
        }
        else if (qualifier == 0x01) // файл принят целиком - готово
        {
            const auto size = static_cast<quint64>(payload.size());
            m_currentWriteSection = 1;
            emit responseParsed(DataTypes::GeneralResponseStruct { DataTypes::GeneralResponseTypes::Ok, size });
            emit requestedDataReceived();
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
    // Новая команда - любой ACTCON/ACTTERM, недополученный ПРЕДЫДУЩЕЙ командой (например,
    // устройство ушло в ResetSystem() сразу после ACTCON, не прислав ACTTERM - см. m_actTerminated),
    // больше не актуален и не должен "засчитаться" вместе с ACTCON/ACTTERM этой новой команды.
    m_actConfirmed = false;
    m_actTerminated = false;
    m_currentCommand = currCommand;
}

} // namespace Interface
