#pragma once

#include <interfaces/types/common_types.h>
#include <interfaces/types/iec104/apci.h>
#include <interfaces/types/iec104/asdu_unpacker.h>

constexpr quint32 BSIGROUP = 1;
constexpr quint32 STARTUPGROUP = 2;
constexpr quint32 ALARMGROUP = 3;
constexpr quint32 MAINFLOATGROUP = 4;
constexpr quint32 MAINBITSTRINGGROUP = 4;
constexpr quint32 TIMEGROUP = 15;

namespace Iec104
{

enum class Command : std::uint8_t
{
    RequestGroup = 0,
    RequestFile,
    RequestConfig,
    WriteFile,
    Command45,
    Command50,
    Command51,
    None = std::numeric_limits<std::uint8_t>::max()
};

/// \brief Reactive reply to send in response to a file transfer step received from the device.
enum class FileReplyAction : std::uint8_t
{
    Select,         ///< Select file - начало передачи.
    CallFile,       ///< Call file - подтверждение готовности файла, запрос содержимого.
    CallSection,    ///< Call section - запрос секции файла.
    ConfirmSection, ///< Confirm section - подтверждение приёма несекции (не последней) файла.
    ConfirmFile     ///< Confirm file - подтверждение приёма файла целиком.
};

/// \brief Reactive reply to send in response to a write-file step requested by the device.
enum class FileWriteReplyAction : std::uint8_t
{
    SectionReady, ///< Section ready - ответ на запрос файла устройством, передаём размер.
    SendSegments, ///< Send segments - ответ на запрос сегментов устройством, шлём весь поток данных.
    FileFinal     ///< File final - ответ на подтверждение секции устройством, завершаем файл.
};

/// \brief Квалификатор выбора и вызова (SCQ) - код действия в F_SC_NA_1 (выбор/запрос файла или секции).
/// \see ГОСТ Р МЭК 60870-5-101-2001, §7.2.6.30.
/// \details Значение занимает младший нибл байта; старший нибл в ОТВЕТЕ устройства несёт независимый
/// код ошибки (область недоступна/ошибка CRC/недопустимая услуга/нет такого файла/нет такой секции) -
/// при разборе входящего SCQ сравнивать нужно с этим нибл в отдельности (byte & 0x0F), см.
/// Iec104ResponseParser::handleFileTransferAsdu.
enum class SelectAndCallQualifier : std::uint8_t
{
    SelectFile = 1,       ///< Select file - выбрать файл, начать передачу.
    RequestFile = 2,      ///< Request file - запросить содержимое файла целиком.
    DeactivateFile = 3,   ///< Deactivate file - отменить выбор файла.
    DeleteFile = 4,       ///< Delete file - удалить файл.
    SelectSection = 5,    ///< Select section - выбрать секцию.
    RequestSection = 6,   ///< Request section - запросить содержимое (сегменты) текущей секции.
    DeactivateSection = 7 ///< Deactivate section - отменить выбор секции.
};

/// \brief Квалификатор последней секции или сегмента (LSQ) - код в F_LS_NA_1 (конец секции или файла).
/// \see ГОСТ Р МЭК 60870-5-101-2001, §7.2.6.31.
/// \details Формат байта тот же CP8, что у SCQ/AFQ (см. их описание) - младший нибл действие,
/// старший нибл в ответе устройства независимый код ошибки/подтверждения.
enum class LastSectionQualifier : std::uint8_t
{
    FileTransferNoDeactivation = 1,     ///< Конец файла целиком - последняя секция обработана.
    FileTransferWithDeactivation = 2,   ///< Конец файла с отменой - передача прервана.
    SectionTransferNoDeactivation = 3,  ///< Конец секции (не последней) - секция готова к подтверждению.
    SectionTransferWithDeactivation = 4 ///< Конец секции с отменой.
};

/// \brief Квалификатор подтверждения файла или секции (AFQ) - код в F_AF_NA_1 (подтверждение файла/секции).
/// \see ГОСТ Р МЭК 60870-5-101-2001, §7.2.6.32.
/// \details Формат байта тот же CP8, что у SCQ/LSQ. На реальном устройстве (см. 104_file.h в
/// прошивке AV-TUK) при ОТРИЦАТЕЛЬНОМ подтверждении старший нибл несёт причину отказа:
/// ERRFILE=0x10 (ошибка файла), ERRKS=0x20 (не совпадает контрольная сумма), NOTFUNC=0x30
/// (недопустимая услуга), NOTFILE=0x40 (нет такого файла), NOTSECT=0x50 (нет такой секции).
enum class AckFileQualifier : std::uint8_t
{
    PositiveFile = 1,    ///< POS_FILE - файл принят успешно.
    NegativeFile = 2,    ///< NEG_FILE - файл отклонён (причина в старшем нибле, см. выше).
    PositiveSection = 3, ///< POS_SECT - секция принята успешно.
    NegativeSection = 4  ///< NEG_SECT - секция отклонена (причина в старшем нибле, см. выше).
};

}

Q_DECLARE_METATYPE(Iec104::FileReplyAction)
Q_DECLARE_METATYPE(Iec104::FileWriteReplyAction)
