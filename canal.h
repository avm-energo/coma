#ifndef CANAL_H
#define CANAL_H

#include <QObject>
#include <QByteArray>
#include <QThread>
#include <QTimer>
#include <QLabel>
#include <QMutex>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "publicclass.h"

// Канал связи с модулем

#define CN_TIMEOUT  2000 // таймаут по USB в мс
#define CN_OSCT     1000 // таймаут посылки запроса нестёртых осциллограмм
#define CN_MAXFILESIZE  30000 // максимальный размер выходного файла
#define CN_MAXSEGMENTLENGTH 768 // максимальная длина одного сегмента (0x300)

// Обмен с модулями
#define CN_ResOk   0x11 // ответ "всё в порядке"
#define CN_GBsi    0x21 // чтение блока стартовой информации
#define CN_GBac    0x22 // чтение настроечных коэффициентов
#define CN_GBda    0x23 // чтение текущих данных без настройки
#define CN_GBd     0x24 // чтение блока текущих данных
#define CN_GF      0x25 // чтение файла
#define CN_GBo     0x26 // чтение осциллограмм
#define CN_GBe     0x28 // чтение банка текущих событий
#define CN_GBTe    0x29 // чтение банка технологических событий
#define CN_WBac    0x31 // запись настроечных коэффициентов
#define CN_WF      0x32 // запись файла
#define CN_Start   0x3e // начало посылки
#define CN_MStart  0x3c // начало посылки модуля
#define CN_Gip     0x43 // чтение IP-адреса
#define CN_OscEr   0x45 // стирание всех осциллограмм из Flash-памяти
#define CN_OscPg   0x46 // запрос оставшегося для стирания по команде OscEr числа страниц
#define CN_WHv     0x48 // запись версии аппаратуры модуля/серийного номера/типа платы
#define CN_ResErr  0xF0 // ответ "ошибка"
#define CN_Unk      0xFF // неизвестная команда

#define CN_L0       0x00 // length is 0 bytes
#define CN_L2       0x02 // length is 2 bytes

// определение ошибок канала

#define CN_OK               0 // ошибок нет
#define CN_PORTOPENERROR    1 // ошибка открытия порта
#define CN_NOPORTSERROR     2 // нет портов в системе
#define CN_SEGFAULTERROR    3 // ошибка при приёме сегмента данных на стороне модуля
#define CN_RCVDATAERROR     4 // ошибка при приёме данных (несовпадение длины, неправильный начальный символ и т.п.)
#define CN_TIMEOUTERROR     5 // таймаут при приёме данных
#define CN_RCVLENGTHERROR   6 // некорректная длина блока (принято больше, чем может вместить в себя приёмная переменная)
#define CN_UNKNOWNCMDERROR  7 // неизвестная команда
#define CN_NULLDATAERROR    12 // в канальную процедуру переданы некорректные данные

// определение ошибок модуля

#define CN_MER_NOERR        0x00 // нет ошибок
#define CN_MER_TIMEOUT      0x01 // таймаут при приёме
#define CN_MER_CRCINCOR     0x02 // неверна контрольная сумма принятого файла CN_WF
#define CN_MER_FLASHERR     0x03 // ошибка записи во Flash-память
#define CN_MER_LENGTHERR    0x04 // ошибка длины пакета
#define CN_MER_WRONGCOM     0x05 // неизвестная команда
#define CN_MER_WRONGFORMAT  0x06 // неизвестный формат команды
#define CN_MER_UNKN_Gbd     0x07 // запрошен неизвестный подблок данных
#define CN_MER_UNKN_FILEREQ 0x08 // запрошен неизвестный номер файла
#define CN_MER_UNKN_FILESNT 0x09 // передан неизвестный номер файла
#define CN_MER_NOT_MEZONIN  0x0A // отсутствует дополнительная плата
#define CN_MER_WRONG_MTYPE  0x0B // неверный тип платы
#define CN_MER_NOT_FILE     0x0C // нет файла с таким номером
#define CN_MER_INKN_ERR     0xFF // неизвестная ошибка

class Canal : public QObject
{
    Q_OBJECT
public:
    explicit Canal(QObject *parent = 0);
    ~Canal();

    enum board_types
    {
        BT_NONE,
        BT_BASE,
        BT_MEZONIN
    };

    int result;
    QSerialPortInfo info;
    int baud;
    int ernum;
    bool FirstRun;
    bool NeedToSend, Busy, NeedToFinish;
    quint32 RDSize;

    bool Connect();
    void Disconnect();
    void Send(int command, int board_type=BT_NONE, void *ptr=NULL, quint32 ptrsize=0, quint16 filenum=0, publicclass::DataRec *DRptr=NULL);

signals:
    void WriteDataLength(quint32);
    void WriteDataPos(quint32);
    void ReadBytesSignal(QByteArray);
    void WriteBytesSignal(QByteArray);
    void SendEnd();
    void OscEraseSize(quint32);
    void OscEraseRemaining(quint32);

public slots:
    void Timeout();

private slots:
    void CheckForData();
    void OscTimerTimeout();
    void PortCloseTimeout();
    void Error(QSerialPort::SerialPortError);

private:
    char *outdata;
    QByteArray ReadData;
    QByteArray WriteData;
    QTimer *TTimer, *OscTimer;
    quint16 OscNum;
    int bStep;
    int cmd;
    QLabel *lbl;
    quint16 fnum;
    quint32 RDLength; // длина всей посылки
    quint32 DLength; // длина данных
    quint32 WRLength; // длина всей посылки
    quint32 outdatasize; // размер приёмной области памяти
    quint32 SegLeft; // количество оставшихся сегментов
    quint32 SegEnd; // номер последнего байта в ReadData текущего сегмента
    bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо
    publicclass::DataRec *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    quint8 BoardType;
    bool PortCloseTimeoutSet;
    QSerialPort *port;

    bool InitializePort(QSerialPortInfo &pinfo, int baud);
    void ClosePort();
    void InitiateSend();
    bool WriteDataToPort(QByteArray &ba);
    void ParseIncomeData(QByteArray &ba);
    void Finish(int ernum);
    void SetRDLength();
    void SetWR(QByteArray &);
    bool RDCheckForNextSegment();
    void WRCheckForNextSegment();
    void SendOk();
    void SendErr();
};

extern Canal cn;

#endif // CANAL_H
