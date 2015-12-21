#ifndef PUBLICCLASS_H
#define PUBLICCLASS_H

// Макросы для выдачи сообщений
#define ERMSG(...)     pc.AddErrMsg(publicclass::ER_MSG,__VA_ARGS__)
#define DBGMSG(...)    pc.AddErrMsg(publicclass::DBG_MSG,__VA_ARGS__)
#define INFOMSG(...)   pc.AddErrMsg(publicclass::INFO_MSG,__VA_ARGS__)
#define WARNMSG(...)   pc.AddErrMsg(publicclass::WARN_MSG,__VA_ARGS__)

#define PUBER(a)       ERMSG(publicclass::ER_PUB,__LINE__,a)
#define PUBDBG         DBGMSG(publicclass::ER_PUB,__LINE__)
#define PUBWARN        WARNMSG(publicclass::ER_PUB,__LINE__)
#define PUBINFO(a)     INFOMSG(publicclass::ER_PUB,__LINE__,a)

#define MAXBYTEARRAY    65535
#define MAX_MSG     1000
#define ER_BUFMAX   16
// определение типов модулей
#define MT_C    1
#define MT_D    2
#define MT_A    3
#define MT_E    4

// определение подтипов модулей
#define MTE_2T0N    0
#define MTE_1T1N    1
#define MTE_0T2N    2

// S2: Определение типов элементов данных
#define u8_TYPE		1  //беззнаковый байт
#define s8_TYPE		2  //знаковый байт
#define u16_TYPE	3  //беззнаковое слово
#define s16_TYPE	4	//знаковое слово
#define u32_TYPE	5  //32-х битовое беззнаковое
#define s32_TYPE	6  //32-х битовое знаковое
#define u64_TYPE	7  //64-х битовое беззнаковое
#define s64_TYPE	8  //64-х битовое знаковое
#define float_TYPE	9  //32-х битовое плавающее
#define double_TYPE	10 //64-х битовое плавающее
#define string_TYPE	11 //строка, ограниченная нулём
#define binary_TYPE	12 //любые двоичные данные

// Канал связи с модулем

#define CN_TIMEOUT  2000 // таймаут по USB в мс
#define CN_OSCT     1000 // таймаут посылки запроса нестёртых осциллограмм

// Обмен с модулями
#define CN_ResOk   0x11 // ответ "всё в порядке"
#define CN_GBsi    0x21 // запрос блока стартовой информации
#define CN_Gac     0x22 // запрос настроечных коэффициентов
#define CN_Gda     0x23 // запрос текущих данных без настройки
#define CN_GBd     0x24 // запрос блока текущих данных
#define CN_GF      0x25 // запрос файла конфигурации
#define CN_GNosc   0x26 // запрос информации об осциллограммах
#define CN_GBosc   0x27 // запрос осциллограммы
#define CN_GBev    0x28 // запрос банка событий
#define CN_GBdi    0x29 // запрос текущих данных, приведённых ко входу
#define CN_Wac     0x31 // запись настроечных коэффициентов
#define CN_WF      0x32 // запись файла конфигурации
#define CN_Wsn     0x33 // запись серийного номера модуля
#define CN_WHv     0x34 // запись версии аппаратуры модуля
#define CN_OscEr   0x35 // стирание всех осциллограмм из Flash-памяти
#define CN_OscPg   0x36 // запрос оставшегося для стирания по команде OscEr числа страниц
#define CN_Start   0x3e // начало посылки
#define CN_MStart  0x3c // начало посылки модуля
#define CN_Cln     0x41 // зажечь светодиод
#define CN_Clf     0x42 // погасить светодиод
#define CN_Gip     0x43 // запрос IP-адреса
#define CN_Cps     0x44 // контроль 1PPS
#define CN_Cnc     0x51 // команда перехода на новую конфигурацию
#define CN_SegOk   0x55 // ответ "сегмент принят в порядке"
#define CN_ResErr  0x0f // ответ "ошибка"
#define CN_Unk     0xff // неизвестная команда
// определение ошибок

#define CN_OK               0 // ошибок нет
#define CN_PORTOPENERROR    1 // ошибка открытия порта
#define CN_NOPORTSERROR     2 // нет портов в системе
#define CN_SEGFAULTERROR    3 // ошибка при приёме сегмента данных на стороне модуля
#define CN_RCVDATAERROR     4 // ошибка при приёме данных (несовпадение длины, неправильный начальный символ и т.п.)
#define CN_TIMEOUTERROR     5 // таймаут при приёме данных
#define CN_RCVLENGTHERROR   6 // некорректная длина блока (принято больше, чем может вместить в себя приёмная переменная)
#define CN_UNKNOWNCMDERROR  7 // неизвестная команда
#define CN_S2SIZEERROR      8 // ошибка длины при работе с форматом S2
#define CN_S2DESCERROR      9 // несовпадение описания прочитанного элемента с ожидаемым при работе с форматом S2
#define CN_S2CRCERROR       10 // несовпадение контрольной суммы при работе с форматом S2
#define CN_S2DHSZERROR      11 // некорректная длина в FileHeader при разборе формата S2
#define CN_NULLDATAERROR    12 // в канальную процедуру переданы некорректные данные
#define CN_NOIDS            13 // не найдено ни одного элемента с заданным ID
#define ER_FILEOPENERROR    15 // ошибка при открытии файла
#define ER_NOCONF           20 // нет конфигурации в модуле
#define ER_NOTUNECOEF       21 // нет настроечных параметров в модуле

// ошибки IP-сокета
#define SKT_UNKNOWNER       24 // неизвестная ошибка
#define SKT_REFUSEDER       25 // тайм-аут при открытии сокета
#define SKT_REMOTECLOSEDER  26 // удалённый узел разорвал соединение
#define SKT_NOTFOUNDER      27 // узел не найден
#define SKT_ACCESSER        28 // ошибка доступа к сокету
#define SKT_RESER           29 // недостаточно памяти для сокета
#define SKT_TOER            30 // тайм-аут при операции с сокетом
#define SKT_LARGEER         31 // слишком большой размер датаграммы
#define SKT_NETWORKER       32 // обрыв связи
#define SKT_ADDRESSER       33 // адрес уже используется
#define SKT_BINDER          34 // адрес привязки не принадлежит данному ПК
#define SKT_UNSUPER         35 // неподдерживаемая операция
#define SKT_PROXYAUTHER     36 // прокси-сервер требует авторизацию
#define SKT_SSLER           37 // ошибка при установлении защищённого соединения
#define SKT_UNFINISHER      38 // есть незавершённая операция
#define SKT_PROXYREFER      39 // прокси-сервер отклонил соединение
#define SKT_PROXYCLOSER     40 // прокси-сервер закрыл соединение
#define SKT_PROXYTOER       41 // произошёл тайм-аут при работе с прокси-сервером
#define SKT_PROXYNOTFOUNDER 42 // прокси-сервер не найден
#define SKT_PROXYPROTER     43 // ответ прокси-сервера не распознан
#define SKT_OPERER          44 // сокет занят
#define SKT_SSLINTER        45 // ошибка SSL на стороне ПК
#define SKT_SSLINVDATAER    46 // некорректные данные SSL
#define SKT_TEMPER          47 // повторите операцию позже
#define SKT_SENDDATAER      48 // ошибка при отправке данных

// ошибки USB (COM) порта
#define COM_DEVNOTFOUNDER   51 // устройство не найдено
#define COM_PERMER          52 // устройство занято другим процессом или недостаточно прав
#define COM_OPENER          53 // устройство уже открыто
#define COM_PARITYER        54 // ошибка чётности
#define COM_FRAMER          55 // ошибка фрейма
#define COM_BREAKER         56 // передача прервана
#define COM_WRITEER         57 // ошибка записи
#define COM_READER          58 // ошибка чтения
#define COM_RESER           59 // устройство внезапно пропало
#define COM_UNSUPER         60 // операция не поддерживается Вашей ОС
#define COM_UNKER           61 // неизвестная ошибка
#define COM_TIMEOUTER       62 // произошло превышение времени ожидания
#define COM_NOTOPENER       63 // операция не может быть выполнена, порт закрыт

// ошибки в модулях УСО
#define USO_UNKER           71 // неизвестная ошибка
#define USO_TIMEOUTER       72 // таймаут
#define USO_CRCER           73 // ошибка CRC
#define USO_FLASHER         74 // ошибка доступа к FLASH-памяти
#define USO_LENGTHER        75 // ошибка длины
#define USO_CMDER           76 // ошибка команды
#define USO_CMDFMTER        77 // ошибка формата команды
#define USO_GBDER           78 // ошибка в блоке данных команды GBd
#define USO_RFILER          79 // запрошен неизвестный номер файла
#define USO_WFILER          80 // передан файл с неизвестным номером

// ошибки протокола МЭК 60870-5-104
#define M104_LENGTHER       81 // ошибка длины в блоке APDU
#define M104_NUMER          82 // ошибка номеров сообщений

#define HTH_FNC             0x00000200 // частота не в допуске (Э)
#define HTH_LS              0x00000100 // сигналы малы (Э)
#define HTH_CONFIG          0x00000080 // в модуле нет конфигурации
#define HTH_REGPARS         0x00000040 // в модуле нет настроечных параметров
#define HTH_ADC             0x00000020 // проблема с АЦП (Э)
#define HTH_1PPS            0x00000010 // нет сигнала 1PPS
#define HTH_REL             0x00000008 // неисправность выходных реле (Д)
#define HTH_TUPP            0x00000004 // перегрев модуля

#define CS_MSGTRIG          500 // частота мигания сообщения о попытке восстановить связь с модулем
#define CS_TIMEOUT          5000 // количество мс ожидания появления связи с модулем

#define TABCOLOR    "rgba(175,255,168,255)"
#define ERPROTCLR   "rgba(248,214,255,255)"
//#define MAINWINCLR  "#CCCC99"
#define MAINWINCLR  "#E0FFE0"
//#define ACONFCLR    "#DDDDAA"
#define ACONFCLR    "#D0FFD0"
//#define ACONFWCLR   "#CCFF99"
#define ACONFWCLR   "#C0FFC0"
//#define ACONFGCLR   "#CCCC99"
#define ACONFGCLR   "#B0FFB0"
//#define ACONFRCLR   "#FFCCCC"
#define ACONFRCLR   "#FFDDDD"
//#define ACONFYCLR   "#FFFFCC"
#define ACONFYCLR   "#FFFFDD"
//#define ACONFOCLR   "#FFCC99"
#define ACONFOCLR   "#FFDDAA"

#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include <QMap>
#include "serialthread.h"

class publicclass
{
public:
    publicclass();

    QSerialPort serial;
    qint32 MType, MType1, SerNum;
    quint32 CpuIdHigh, CpuIdMid, CpuIdLow, Health;

    QStringList AMTypes, DMTypes, EMTypes;
    SerialThread *SThread;
    quint16 MIPASDU;
    QString MIPIP;
    QString Port;
    int result;
    QStringList errmsgs;
    QString ermsgpath;
    QString ModuleTypeString;
    bool Emul;
    int ErrWindowDelay;
    bool ShowErrWindow;

    // S2: Определение типа заголовка
    typedef struct
    {
      quint32 size;
      quint32 crc32;
      quint32 thetime;
    } FileHeader;

    // S2: Определение типа записи

    typedef struct DataRec
    {
      quint16 id;
      quint16 data_type;
      quint32 elem_size;
      quint32 num_elem;
      void *thedata;
    } DataRec;

    typedef struct
    {
        qint32 MType;
        qint32 MType1;
        qint32 HWver;
        qint32 FWver;
        qint32 Cfcrc;
        qint32 Rst;
        qint32 RstCount;
        qint32 Hth;
        qint32 CpuIdLow;
        qint32 CpuIdMid;
        qint32 CpuIdHigh;
        qint32 SerNum;
    } Bsi;

    enum ermsgtype
    {
        ER_MSG,
        WARN_MSG,
        INFO_MSG,
        DBG_MSG
    };

    struct ermsg
    {
        QString DateTime;
        ermsgtype type;
        quint64 ernum;
        quint64 ersubnum;
        QString msg;
    };

    enum errors
    {
        ER_MAIN,
        ER_CANAL,
        ER_SERIAL,
        ER_ETUNE,
        ER_ECONF,
        ER_ATUNE,
        ER_ACONF,
        ER_OSC,
        ER_PUB
    };

    static QMap<int, QString> ermsgs()
    {
        QMap<int, QString>map;
        map.insert(ER_MAIN, "Кома");
        map.insert(ER_CANAL, "Канал");
        map.insert(ER_SERIAL, "Поток USB");
        map.insert(ER_ETUNE, "Настр. Э");
        map.insert(ER_ECONF, "Конф. Э");
        map.insert(ER_ATUNE, "Настр. A");
        map.insert(ER_ACONF, "Конф. A");
        map.insert(ER_OSC, "Осциллограммы");
        map.insert(ER_PUB, "Основное");
        return map;
    }
    QList<ermsg> ermsgpool;

    QString VerToStr(quint32);
    quint32 ANumD();
    quint32 ANumCh1();
    quint32 ANumCh2();
    quint32 ATyp1();
    quint32 ATyp2();
    quint32 AMdf();
    QString ETyp1();

    // S2: Сборщик в память:
    int StoreDataMem(void *, DataRec *); //0 - успешно, иначе код ошибки
    // S2: получение размера:
    int StoreDataSize(FileHeader *, DataRec *); //>0 - успешно, иначе код ошибки
    // S2: Разборщик из памяти:
    int RestoreDataMem(void *, quint32, DataRec *); //0 - успешно, иначе код ошибки
    // S2: Поиск элемента в массиве описаний
    DataRec *FindElem(DataRec *, quint16);

    quint32 GetCRC32(char *, quint32);
    void updCRC32(const quint8 byte, quint32 *dwCRC32);
    quint32 getTime32();
    QString NsTimeToString (quint64 nstime);
    void AddErrMsg(ermsgtype msgtype, quint64 ernum=0, quint64 ersubnum=0, QString msg="");
    QByteArray LoadFile(QString mask);
    int SaveFile (QString mask, void *src, unsigned int numbytes);

private:
    QByteArray *LoadBa, *SaveBa;
    void addmessage(QStringList &sl, QString mes);
    void SetErMsg(int ernum);
};

extern publicclass pc;

#endif // PUBLICCLASS_H
