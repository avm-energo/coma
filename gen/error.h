#ifndef ERROR_H
#define ERROR_H

#include <QString>
#include <QList>

#include "log.h"

// Макросы для выдачи сообщений
#define ERMSG(a)    Error::AddErrMsg(Error::ER_MSG,__FILE__,__LINE__,a)
#define DBGMSG      Error::AddErrMsg(Error::DBG_MSG,__FILE__,__LINE__,"")
#define INFOMSG(a)  Error::AddErrMsg(Error::INFO_MSG,__FILE__,__LINE__,a)
#define WARNMSG(a)  Error::AddErrMsg(Error::WARN_MSG,__FILE__,__LINE__,a)

#define USB_COMER           1 // USB: Ошибка открытия COM-порта
#define USB_NOCOMER         2 // USB: В системе нет COM-портов
#define USB_MODSEGER        3 // USB: Ошибка при приёме сегмента данных на стороне модуля
#define USB_READER          4 // USB: Ошибка при приёме данных
#define USB_READTIMEOUTER   5 // USB: Произошло превышение времени ожидания при приёме данных
#define USB_READWRLENER     6 // USB: Некорректная длина принятого блока
#define USB_WRONGCOMER      7 // USB: Неизвестная команда
// ошибки разбора S2
#define S2_SIZEERROR        8 // ошибка длины при работе с форматом S2
#define S2_DESCERROR        9 // несовпадение описания прочитанного элемента с ожидаемым при работе с форматом S2
#define S2_CRCERROR         10 // несовпадение контрольной суммы при работе с форматом S2
#define S2_DHSZERROR        11 // некорректная длина в FileHeader при разборе формата S2
#define S2_NOIDS            13 // не найдено ни одного элемента с заданным ID

// ошибки модулей
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
#define CN_NOMAPFILE        79 // невозможно отмаппить временный файл в память

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
#define USO_WRONGBAC        94 // ошибка блока данных Bac

// ошибки протокола МЭК 60870-5-104
#define M104_ERROR          100
#define M104_LENGTHER       101 // ошибка длины в блоке APDU
#define M104_NUMER          102 // ошибка номеров сообщений

#define ER_BUFMAX   16
#define LOGFILE     "coma.log"

#define NOERROR         0 // нет ошибок
#define GENERALERROR    -1 // возвращённое значение - ошибка
#define RESEMPTY        -2 // пустой результат


class Error
{
public:
    enum ErMsgType
    {
        ER_MSG,
        WARN_MSG,
        INFO_MSG,
        DBG_MSG
    };

    struct ErMsg
    {
        QString DateTime;
        ErMsgType type;
        QString file;
        int line;
        QString msg;
    };

    Error();
    static void Init();
    static void AddErrMsg(ErMsgType msgtype, QString file=nullptr, int line=0, QString msg="");
    static void ShowErMsg(int ermsgnum);
    static int ErMsgPoolSize();
    static ErMsg ErMsgAt(int idx);

private:
    static QStringList ErrMsgs;
    static QList<ErMsg> ErMsgPool;
    static Log LogFile;

    void addmessage(QStringList &sl, QString mes);
    void SetErMsg(int ernum);
};

#endif // ERROR_H
