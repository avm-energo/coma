#pragma once

#include <QObject>
#include <QString>
// Макросы для выдачи сообщений
#define ERMSG qCritical
#define DBGMSG qDebug
#define INFOMSG qInfo
#define WARNMSG qWarning

class Error : public QObject
{
    Q_OBJECT
public:
    struct ErMsg
    {
        QString DateTime;
        QtMsgType type;
        QString file;
        int line;
        QString msg;
    };

    enum class Msg
    {
        // пустой результат
        ResEmpty = -2,
        // возвращённое значение - ошибка
        GeneralError,
        // нет ошибок
        NoError,

        // USB: Ошибка открытия COM-порта
        USB_COMER,
        // USB: В системе нет COM-портов
        USB_NOCOMER,
        // USB: Ошибка при приёме сегмента данных на стороне модуля
        USB_MODSEGER,
        // USB: Ошибка при приёме данных
        USB_READER,
        // USB: Произошло превышение времени ожидания при приёме данных
        USB_READTIMEOUTER,
        // USB: Некорректная длина принятого блока
        USB_READWRLENER,
        // USB: Неизвестная команда
        USB_WRONGCOMER,

        // ошибки разбора S2
        // ошибка длины при работе с форматом S2
        S2_SIZEERROR,
        // несовпадение описания прочитанного элемента с ожидаемым при работе с форматом S2
        S2_DESCERROR,
        // несовпадение контрольной суммы при работе с форматом S2
        S2_CRCERROR,
        // некорректная длина в FileHeader при разборе формата S2
        S2_DHSZERROR,
        // не найдено ни одного элемента с заданным ID
        S2_NOIDS,

        // ошибки модулей
        // ошибка при открытии файла
        ER_FILEOPENERROR,
        // нет конфигурации в модуле
        ER_NOCONF,
        // нет настроечных параметров в модуле
        ER_NOTUNECOEF,

        // ошибки IP-сокета
        // неизвестная ошибка
        SKT_UNKNOWNER,
        // тайм-аут при открытии сокета
        SKT_REFUSEDER,
        // удалённый узел разорвал соединение
        SKT_REMOTECLOSEDER,
        // узел не найден
        SKT_NOTFOUNDER,
        // ошибка доступа к сокету
        SKT_ACCESSER,
        // недостаточно памяти для сокета
        SKT_RESER,
        // тайм-аут при операции с сокетом
        SKT_TOER,
        // слишком большой размер датаграммы
        SKT_LARGEER,
        // обрыв связи
        SKT_NETWORKER,
        // адрес уже используется
        SKT_ADDRESSER,
        // адрес привязки не принадлежит данному ПК
        SKT_BINDER,
        // неподдерживаемая операция
        SKT_UNSUPER,
        // прокси-сервер требует авторизацию
        SKT_PROXYAUTHER,
        // ошибка при установлении защищённого соединения
        SKT_SSLER,
        // есть незавершённая операция
        SKT_UNFINISHER,
        // прокси-сервер отклонил соединение
        SKT_PROXYREFER,
        // прокси-сервер закрыл соединение
        SKT_PROXYCLOSER,
        // произошёл тайм-аут при работе с прокси-сервером
        SKT_PROXYTOER,
        // прокси-сервер не найден
        SKT_PROXYNOTFOUNDER,
        // ответ прокси-сервера не распознан
        SKT_PROXYPROTER,
        // сокет занят
        SKT_OPERER,
        // ошибка SSL на стороне ПК
        SKT_SSLINTER,
        // некорректные данные SSL
        SKT_SSLINVDATAER,
        // повторите операцию позже
        SKT_TEMPER,
        // ошибка при отправке данных
        SKT_SENDDATAER,

        // ошибки USB (COM) порта
        COM_ERROR,
        // устройство не найдено
        COM_DEVNOTFOUNDER,
        // устройство занято другим процессом или недостаточно прав
        COM_PERMER,
        // устройство уже открыто
        COM_OPENER,
        // ошибка чётности
        COM_PARITYER,
        // ошибка фрейма
        COM_FRAMER,
        // передача прервана
        COM_BREAKER,
        // ошибка записи
        COM_WRITEER,
        // ошибка чтения
        COM_READER,
        // устройство внезапно пропало
        COM_RESER,
        // операция не поддерживается Вашей ОС
        COM_UNSUPER,
        // неизвестная ошибка
        COM_UNKER,
        // произошло превышение времени ожидания
        COM_TIMEOUTER,
        // операция не может быть выполнена, порт закрыт
        COM_NOTOPENER,

        // ошибки канала
        CN_ERROR,
        // ошибка открытия порта
        CN_PORTOPENERROR,
        // нет портов в системе
        CN_NOPORTSERROR,
        // ошибка при приёме сегмента данных на стороне модуля
        CN_SEGFAULTERROR,
        // ошибка при приёме данных (несовпадение длины, неправильный начальный символ и т.п.)
        CN_RCVDATAERROR,
        // таймаут при приёме данных
        CN_TIMEOUTERROR,
        // некорректная длина блока (принято больше, чем может вместить в себя приёмная  переменная)

        CN_RCVLENGTHERROR,
        // неизвестная команда
        CN_UNKNOWNCMDERROR,
        // в канальную процедуру переданы некорректные данные
        CN_NULLDATAERROR,
        // невозможно отмаппить временный файл в память
        CN_NOMAPFILE,

        // ошибки в модулях УСО
        USO_ERROR,
        // нет ошибки
        USO_NOERR,
        // таймаут при приёме
        USO_TIMEOUTER,
        // неверна контрольная сумма принятого файла CN_WF
        USO_CRCINCOR,
        // ошибка записи во Flash-память
        USO_FLASHERR,
        // ошибка длины пакета
        USO_LENGTHERR,
        // неизвестная команда
        USO_WRONGCOM,
        // неизвестный формат команды
        USO_WRONGFORMAT,
        // запрошен неизвестный подблок данных
        USO_UNKNGBD,
        // запрошен неизвестный номер файла
        USO_UNKNFILEREQ,
        // передан файл с неизвестным номером
        USO_UNKNFILESENT,
        // отсутствует дополнительная плата
        USO_NOTMEZZANINE,
        // неверный тип платы
        USO_WRONGMTYPE,
        // нет файла с таким номером
        USO_NOTFILE,
        // ошибка блока данных Bac
        USO_WRONGBAC,

        // ошибки протокола МЭК 60870-5-104
        M104_ERROR,
        // ошибка длины в блоке APDU
        M104_LENGTHER,
        // ошибка номеров сообщений
        M104_NUMER,

        // ошибка записи файла
        FILE_WRITE,
        // пустое имя файла
        FILE_NAMEEMP,
        // ошибка открытия файл
        FILE_OPEN
    };
    Q_ENUM(Msg)
};

QStringList &operator<<(QStringList &l, const Error::ErMsg &obj);
