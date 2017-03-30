#ifndef PUBLICCLASS_H
#define PUBLICCLASS_H

// Макросы для выдачи сообщений
#define ERMSG(a)    pc.AddErrMsg(publicclass::ER_MSG,__FILE__,__LINE__,a)
#define DBGMSG      pc.AddErrMsg(publicclass::DBG_MSG,__FILE__,__LINE__,"")
#define INFOMSG(a)  pc.AddErrMsg(publicclass::INFO_MSG,__FILE__,__LINE__,a)
#define WARNMSG(a)  pc.AddErrMsg(publicclass::WARN_MSG,__FILE__,__LINE__,a)

#define MAXBYTEARRAY    65535
#define MAX_MSG     1000
#define ER_BUFMAX   16

#define GENERALERROR    -1 // возвращённое значение - ошибка

/*// S2: Определение типов элементов данных
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
#define binary_TYPE	12 //любые двоичные данные */

// определение ошибок
#define NOERROR             0 // нет ошибок

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
#define COM_ERROR           50
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

// ошибки канала
#define CN_ERROR            70
#define CN_PORTOPENERROR    71 // ошибка открытия порта
#define CN_NOPORTSERROR     72 // нет портов в системе
#define CN_SEGFAULTERROR    73 // ошибка при приёме сегмента данных на стороне модуля
#define CN_RCVDATAERROR     74 // ошибка при приёме данных (несовпадение длины, неправильный начальный символ и т.п.)
#define CN_TIMEOUTERROR     75 // таймаут при приёме данных
#define CN_RCVLENGTHERROR   76 // некорректная длина блока (принято больше, чем может вместить в себя приёмная переменная)
#define CN_UNKNOWNCMDERROR  77 // неизвестная команда
#define CN_NULLDATAERROR    78 // в канальную процедуру переданы некорректные данные

// ошибки в модулях УСО
#define USO_ERROR           80
#define USO_NOERR           81 // нет ошибки
#define USO_TIMEOUTER       82 // таймаут при приёме
#define USO_CRCINCOR        83 // неверна контрольная сумма принятого файла CN_WF
#define USO_FLASHERR        84 // ошибка записи во Flash-память
#define USO_LENGTHERR       85 // ошибка длины пакета
#define USO_WRONGCOM        86 // неизвестная команда
#define USO_WRONGFORMAT     87 // неизвестный формат команды
#define USO_UNKNGBD         88 // запрошен неизвестный подблок данных
#define USO_UNKNFILEREQ     89 // запрошен неизвестный номер файла
#define USO_UNKNFILESENT    90 // передан файл с неизвестным номером
#define USO_NOTMEZZANINE    91 // отсутствует дополнительная плата
#define USO_WRONGMTYPE      92 // неверный тип платы
#define USO_NOTFILE         93 // нет файла с таким номером

// ошибки протокола МЭК 60870-5-104
#define M104_ERROR          100
#define M104_LENGTHER       101 // ошибка длины в блоке APDU
#define M104_NUMER          102 // ошибка номеров сообщений

#define HTH_FNC             0x00000200 // частота не в допуске (Э)
#define HTH_LS              0x00000100 // сигналы малы (Э)
#define HTH_CONFIG          0x00000080 // в модуле нет конфигурации
#define HTH_REGPARS         0x00000040 // в модуле нет настроечных параметров
#define HTH_ADC             0x00000020 // проблема с АЦП (Э)
#define HTH_1PPS            0x00000010 // нет сигнала 1PPS
#define HTH_REL             0x00000008 // неисправность выходных реле (Д)
#define HTH_TUPP            0x00000004 // перегрев модуля

// ошибки разбора S2
#define S2_SIZEERROR      8 // ошибка длины при работе с форматом S2
#define S2_DESCERROR      9 // несовпадение описания прочитанного элемента с ожидаемым при работе с форматом S2
#define S2_CRCERROR       10 // несовпадение контрольной суммы при работе с форматом S2
#define S2_DHSZERROR      11 // некорректная длина в FileHeader при разборе формата S2
#define S2_NOIDS          13 // не найдено ни одного элемента с заданным ID

// ошибки модулей
#define ER_OK               0
#define ER_FILEOPENERROR    15 // ошибка при открытии файла
#define ER_NOCONF           20 // нет конфигурации в модуле
#define ER_NOTUNECOEF       21 // нет настроечных параметров в модуле

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
#define ERRCLR      "#FF0000"

#define FLOAT_THRESHOLD 0.1
#define ANMEASINT   2000

#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include <QMap>

class publicclass
{
public:
    publicclass();

    // S2: Определение типа заголовка
    typedef struct
    {
        quint16 fname;
        quint16 service;
        quint32 size;
        quint32 crc32;
        quint32 thetime;
    } FileHeader;

    // S2: Определение типа записи

    typedef struct DataRec
    {
      quint32 id;
      quint32 num_byte;
      void *thedata;
    } DataRec;

    typedef struct
    {
        qint32 MTypeB;
        qint32 MTypeM;
        qint32 HwverB;
        qint32 Fwver;
        qint32 Cfcrc;
        qint32 Rst;
        qint32 RstCount;
        qint32 Hth;
        qint32 UIDLow;
        qint32 UIDMid;
        qint32 UIDHigh;
        qint32 SerialNumB;
        qint32 SerialNumM;
        qint32 HwverM;
        qint32 SerialNum;
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
        QString file;
        int line;
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
        ER_PUB,
        ER_ACHECK,
        ER_ECHECK
    };

    QSerialPort serial;
//    qint32 MType, MType1, SerNum;
//    quint32 CpuIdHigh, CpuIdMid, CpuIdLow, Health;

    QStringList AMTypes, DMTypes, EMTypes;
    quint16 MIPASDU;
    QString MIPIP;
    QString Port;
    int result;
    QStringList errmsgs;
    QString ermsgpath;
    QString ModuleTypeString;
    bool Emul;
    int ErrWindowDelay;
    bool ShowErrWindow, ErMsgsOk;
    Bsi ModuleBsi;
    QString PrbMessage;

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
    int StoreDataMem(void *, DataRec *, quint16 fname); //0 - успешно, иначе код ошибки
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
    void AddErrMsg(ermsgtype msgtype, QString file=0, int line=0, QString msg="");
    QByteArray LoadFile(QString mask);
    int SaveFile (QString mask, void *src, unsigned int numbytes);
    bool FloatInRange(float var, float value);

private:
    QByteArray *LoadBa, *SaveBa;
    void addmessage(QStringList &sl, QString mes);
    void SetErMsg(int ernum);
};

extern publicclass pc;

#endif // PUBLICCLASS_H
