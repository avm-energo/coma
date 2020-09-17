#pragma once
// Обмен с модулями
// Канал связи с модулем
namespace CN
{
typedef unsigned char byte;
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
    // минимальный ИД осциллограмм, нужно, т.к. файлы осциллограмм обрабатываются по-своему
    constexpr unsigned MinOscillogramId = 1000;

    // максимальный ИД журналов
    constexpr byte MaxJournalId = 6;
    // минимальный ИД журналов
    constexpr byte MinJournalId = 4;
}
// Канал связи с модулем
// таймаут по USB в мс
constexpr unsigned Timeout = 1000;
// таймаут посылки запроса нестёртых осциллограмм
constexpr unsigned TimeoutOscillogram = 1000;
// 100 ms main loop sleep
constexpr unsigned MainLoopDelay = 100;
//'\x00'
constexpr byte NullByte = 0x00;
// ответ "всё в порядке"
constexpr byte ResultOk = 0x11;
// запуск, остановка теста
constexpr byte Test = 0x49;
// ответ "ошибка"
constexpr byte ResultError = 0xf0;
// неизвестная команда
constexpr byte Unknown = 0xff;
namespace Read
{
    // чтение блока стартовой информации
    constexpr byte BlkStartInfo = 0x21;
    // чтение настроечных коэффициентов
    constexpr byte BlkAC = 0x22;
    // чтение текущих данных без настройки
    constexpr byte BlkDataA = 0x23;
    // чтение блока текущих данных
    constexpr byte BlkData = 0x24;
    // чтение технологического блока
    constexpr byte BlkTech = 0x26;
    // чтение файла
    constexpr byte File = 0x25;
    // чтение номера варианта использования
    constexpr byte Variant = 0x27;
    // чтение текущего режима работы
    constexpr byte Mode = 0x28;
    // чтение времени из МНК в формате UNIX
    constexpr byte Time = 0x29;
    // запрос текущего прогресса
    constexpr byte Progress = 0x46;

}
namespace Write
{
    // запись настроечных коэффициентов
    constexpr byte BlkAC = 0x31;
    // посылка блока данных
    constexpr byte BlkData = 0x34;
    // посылка команды
    constexpr byte BlkCmd = 0x35;
    // запись технологического блока
    constexpr byte BlkTech = 0x2B;
    // запись файла
    constexpr byte File = 0x32;
    // задание варианта использования
    constexpr byte Variant = 0x44;
    // задание текущего режима работы
    constexpr byte Mode = 0x43;
    // запись времени в МНК в формате UNIX
    constexpr byte Time = 0x2A;
    // переход на новое ПО
    constexpr byte Upgrade = 0x40;
    // стирание технологического блока
    constexpr byte EraseTech = 0x45;
    // стирание счётчиков дискретных состояний
    constexpr byte EraseCnt = 0x47;
    // запись версии аппаратуры модуля/серийного номера/типа платы
    constexpr byte Hardware = 0x48;
}
namespace Message
{
    // начало посылки
    constexpr byte Start = 0x3e;
    // продолжение посылки
    constexpr byte Continue = 0x23;
    // начало посылки модуля
    constexpr byte Module = 0x3c;
    // length is 2 bytes
    constexpr byte Length2Byte = 0x02;
}
}
namespace UH
{
constexpr unsigned VID = 0xC251;
constexpr unsigned PID = 0x3505;
// максимальная длина одного сегмента (0x40)
constexpr unsigned MaxSegmenthLength = 64;
// 20 ms main loop sleep
constexpr unsigned MainLoopDelay = 20;

}

struct DeviceConnectStruct
{
    unsigned short vendor_id;
    unsigned short product_id;
    wchar_t serial[20];
};
#define WHV_SIZE_ONEBOARD 17
#define WHV_SIZE_TWOBOARDS 33

//#define TECH_Bd0    0   // блок данных с температурой кристалла и напряжением батареи
#define TECH_Bo 1  // технологический блок осциллограмм
#define TECH_Be 2  // технологический блок событий
#define TECH_Bte 3 // технологический блок технологических событий
#define TECH_SWJ 4 // технологический блок журнала переключений
#define TECH_RA 5  // технологический блок рабочего архива

#define CM_CONFIGFILE 1
#define CM_FIRMWAREFILE 3
#define CM_SWJFILE 17
#define CM_EVENTSFILE 18
#define WORK_MODE 0
#define TUNE_MODE_1000 1
#define TUNE_MODE_100 2

#ifndef __GNUC__
#define MAXFLOAT 3.40282347E+38F
#endif
