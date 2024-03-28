#pragma once

#include <QVariant>
#include <gen/uint24.h>

// Обмен с модулями
// Канал связи с модулем
namespace Proto
{
Q_NAMESPACE

// Канал связи с модулем
// таймаут по USB в мс
constexpr unsigned Timeout = 1000;
// таймаут посылки запроса нестёртых осциллограмм
constexpr unsigned TimeoutOscillogram = 1000;
// максимальный размер одного сегмента по USB
constexpr unsigned MaxSegmenthLength = 60;

enum Starters : std::uint8_t
{
    // начало посылки
    Request = 0x3e,
    // продолжение посылки
    Continue = 0x23,
    // начало посылки модуля
    Response = 0x3c
};

enum Commands : std::uint8_t
{
    ResultOk = 0x11,    ///< Ответ "всё в порядке"
    Test = 0x49,        ///< Запуск, остановка теста
    ResultError = 0xf0, ///< Ответ "ошибка"
    Unknown = 0xff,     ///< Неизвестная команда

    ///
    /// namespace READ
    ///
    ReadBlkStartInfoExt = 0x20, ///< Чтение дополнительного блока стартовой информации
    ReadBlkStartInfo = 0x21, ///< Чтение блока стартовой информации
    ReadBlkAC = 0x22,        ///< Чтение настроечных коэффициентов
    ReadBlkDataA = 0x23,     ///< Чтение текущих данных без настройки
    ReadBlkData = 0x24,      ///< Чтение блока текущих данных
    ReadBlkTech = 0x26,      ///< Чтение технологического блока
    ReadFile = 0x25,         ///< Чтение файла
    ReadVariant = 0x27,      ///< Чтение номера варианта использования
    ReadMode = 0x28,         ///< Чтение текущего режима работы
    ReadTime = 0x29,         ///< Чтение времени из МНК в формате UNIX
    ReadProgress = 0x46,     ///< Запрос текущего прогресса

    ///
    /// namespace WRITE
    ///
    WriteBlkAC = 0x31,         ///< запись настроечных коэффициентов
    WriteBlkData = 0x34,       ///< посылка блока данных
    WriteBlkCmd = 0x35,        ///< посылка команды
    WriteBlkTech = 0x2B,       ///< запись технологического блока
    WriteSingleCommand = 0x2D, ///< отправка команды 104
    WriteFile = 0x32,          ///< запись файла
    WriteVariant = 0x44,       ///< задание варианта использования
    WriteMode = 0x43,          ///< задание текущего режима работы
    WriteTime = 0x2A,          ///< запись времени в МНК в формате UNIX
    WriteUpgrade = 0x40,       ///< переход на новое ПО

    ///
    /// namespace ERASE
    ///
    EraseTech = 0x45, ///< стирание технологического блока
    EraseCnt = 0x47,  ///< стирание счётчиков дискретных состояний
    WriteHardware = 0x48, ///< запись версии аппаратуры модуля/серийного номера/типа платы

    ///
    /// Fake commands
    ///
    FakeReadRegData,
    FakeReadAlarms,
    FakeReadBitString
};
Q_ENUM_NS(Proto::Commands)

}
