#pragma once
#include <QVariant>
// Обмен с модулями
// Канал связи с модулем
typedef unsigned char byte;
namespace Proto
{
Q_NAMESPACE

namespace Limits
{
    // 64-4 ('<',cmd,L,L)
    constexpr unsigned MaxSegmenthLength = 60;
    constexpr unsigned MaxMemoryFileSize = 65535;
    // максимальный размер выходного файла
    constexpr unsigned MaxFileSize = 300000;
    constexpr unsigned MaxGetFileSize = 16777215;

    // максимальный ИД осциллограмм
    constexpr unsigned MaxOscillogramId = 2999;
    // минимальный ИД осциллограмм, нужно, т.к. файлы осциллограмм обрабатываются
    // по-своему
    constexpr unsigned MinOscillogramId = 1000;

    //    // максимальный ИД журналов
    //    constexpr byte MaxJournalId = 6;
    //    // минимальный ИД журналов
    //    constexpr byte MinJournalId = 4;

    //    constexpr byte ConfigFileId = 1;
    //    constexpr byte FirmwareFileId = 3;
    //    constexpr byte SwjFileId = 17;
    //    constexpr byte EventFileId = 18;
} // namespace Limits
// Канал связи с модулем
// таймаут по USB в мс
constexpr unsigned Timeout = 1000;
// таймаут посылки запроса нестёртых осциллограмм
constexpr unsigned TimeoutOscillogram = 1000;
// 100 ms main loop sleep
constexpr unsigned MainLoopDelay = 100;
//'\x00'
constexpr byte NullByte = 0x00;

enum Starters : byte
{
    // начало посылки
    Request = 0x3e,
    // продолжение посылки
    Continue = 0x23,
    // начало посылки модуля
    Response = 0x3c
};

enum Commands : byte
{

    // ответ "всё в порядке"
    ResultOk = 0x11,
    // запуск, остановка теста
    Test = 0x49,
    // ответ "ошибка"
    ResultError = 0xf0,
    // неизвестная команда
    Unknown = 0xff,
    ///
    /// namespace READ
    ///
    // чтение блока стартовой информации
    ReadBlkStartInfo = 0x21,
    // чтение настроечных коэффициентов
    ReadBlkAC = 0x22,
    // чтение текущих данных без настройки
    ReadBlkDataA = 0x23,
    // чтение блока текущих данных
    ReadBlkData = 0x24,
    // чтение технологического блока
    ReadBlkTech = 0x26,
    // чтение файла
    ReadFile = 0x25,
    // чтение номера варианта использования
    ReadVariant = 0x27,
    // чтение текущего режима работы
    ReadMode = 0x28,
    // чтение времени из МНК в формате UNIX
    ReadTime = 0x29,
    // запрос текущего прогресса
    ReadProgress = 0x46,
    ///
    /// namespace WRITE
    ///
    // запись настроечных коэффициентов
    WriteBlkAC = 0x31,
    // посылка блока данных
    WriteBlkData = 0x34,
    // посылка команды
    WriteBlkCmd = 0x35,
    // запись технологического блока
    WriteBlkTech = 0x2B,
    // запись файла
    WriteFile = 0x32,
    // задание варианта использования
    WriteVariant = 0x44,
    // задание текущего режима работы
    WriteMode = 0x43,
    // запись времени в МНК в формате UNIX
    WriteTime = 0x2A,
    // переход на новое ПО
    WriteUpgrade = 0x40,

    ///
    /// namespace ERASE
    ///

    // стирание технологического блока
    EraseTech = 0x45,
    // стирание счётчиков дискретных состояний
    EraseCnt = 0x47,
    // запись версии аппаратуры модуля/серийного номера/типа платы
    WriteHardware = 0x48,
    /// Fake commands
    FakeReadRegData,
    RawCommand = 0x01

};
Q_ENUM_NS(Proto::Commands);

enum WCommands
{
    /// Блок дополнительных команд
    EraseStartupValues = 0x05,
    InitStartupValues = 0x01
};

enum HiddenBlockModule : byte
{
    Base = 0x01,
    Mezz = 0x02,
    BaseMezz = 0x03,
    Add = 0x04,
    BaseMezzAdd = 0x07
};

enum Direction
{
    NoDirection,
    ToDevice,
    FromDevice
};

struct CommandStruct
{
    Proto::Commands cmd;
    QVariant arg1;
    QVariant arg2;
    QByteArray ba;
};

enum TypeId : int
{
    Bool,
    Uint32,
    Float,
    File,
    ByteArray
    // Smth else
};
Q_ENUM_NS(TypeId)

// GBsi,ErPg - bitstring,
// GBac, GBda, GBd,GBt - float,
// GF - file
// Список регистров BSI
const QList<quint16> bsiReg { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

// Получаем номер блока по номеру регистра
// Количество регистров необходимо для проверки
// это дело уехало в xml
// const QMap<quint16, QPair<quint8, quint16>> getBlkByReg {
//    { 2420, { 4, 14 } },  //
//    { 2400, { 5, 7 } },   //
//    { 4501, { 8, 2 } },   //
//    { 1000, { 2, 16 } },  //
//    { 1100, { 3, 16 } },  //
//    { 101, { 0, 2 } },    //
//    { 4000, { 7, 12 } },  //
//    { 3011, { 100, 25 } } //
//};

}
Q_DECLARE_METATYPE(Proto::CommandStruct)

//#define WORK_MODE 0
//#define TUNE_MODE_1000 1
//#define TUNE_MODE_100 2

#define HIDUSB_LOG
//#define PROTOCOM_DEBUG

//#define TECH_Bd0    0   // блок данных с температурой кристалла и напряжением
//батареи
#define TECH_Bo 1  // технологический блок осциллограмм
#define TECH_Be 2  // технологический блок событий
#define TECH_Bte 3 // технологический блок технологических событий
#define TECH_SWJ 4 // технологический блок журнала переключений
#define TECH_RA 5  // технологический блок рабочего архива

constexpr int alarm_reg = 3011;
