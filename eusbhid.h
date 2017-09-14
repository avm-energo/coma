#ifndef EUSBHID_H
#define EUSBHID_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QMutex>

#include "publicclass.h"
#include "log.h"
#include "eabstractprotocomchannel.h"
#include "hidapi/hidapi.h"

// Канал связи с модулем

#define UH_TIMEOUT  2000 // таймаут по USB в мс
#define UH_OSCT     1000 // таймаут посылки запроса нестёртых осциллограмм
#define UH_MAXFILESIZE  30000 // максимальный размер выходного файла
#define UH_MAXSEGMENTLENGTH 64 // максимальная длина одного сегмента (0x40)
#define UH_MAINLOOP_DELAY   50 // 100 ms main loop sleep

#define UH_VID  0xC251
#define UH_PID  0x3505

// Обмен с модулями
#define CN_BYTE0    '\x00'
#define CN_ResOk    0x11 // ответ "всё в порядке"
#define CN_GBsi     0x21 // чтение блока стартовой информации
#define CN_GBac     0x22 // чтение настроечных коэффициентов
#define CN_GBda     0x23 // чтение текущих данных без настройки
#define CN_GBd      0x24 // чтение блока текущих данных
#define CN_GF       0x25 // чтение файла
#define CN_GBo      0x26 // чтение осциллограмм
#define CN_GBe      0x28 // чтение банка текущих событий
#define CN_GBTe     0x29 // чтение банка технологических событий
#define CN_WBac     0x31 // запись настроечных коэффициентов
#define CN_WF       0x32 // запись файла
#define CN_IP       0x43 // чтение IP-адреса
#define CN_OscEr    0x45 // стирание всех осциллограмм из Flash-памяти
#define CN_OscPg    0x46 // запрос оставшегося для стирания по команде OscEr числа страниц
#define CN_CtEr     0x47 // стирание счётчиков дискретных состояний
#define CN_WHv      0x48 // запись версии аппаратуры модуля/серийного номера/типа платы
#define CN_ResErr   '\xF0' // ответ "ошибка"
#define CN_Unk      '\xFF' // неизвестная команда

#define CN_MS       0x3e // начало посылки
#define CN_MS3      0x23 // продолжение посылки
#define CN_SS       0x3c // начало посылки модуля
#define CN_L2       0x02 // length is 2 bytes

// определение ошибок модуля - см. publicclass.h (USO_)

#define CN_MER_UNKN_ERR     0xFF // неизвестная ошибка

//#define MAXLENGTH   0x300   // максимальный размер блока 768 байт
#define UH_MAXLENGTH   0x40   // максимальный размер блока 64 байт

#define BT_NONE     0
#define BT_BASE     1
#define BT_MEZONIN  2

class EUsbThread : public QObject
{
    Q_OBJECT
public:
    explicit EUsbThread(QObject *parent = 0);
    ~EUsbThread();

    Log *log;

    int Set();

signals:
    void NewDataReceived(QByteArray ba);

public slots:
    void Run();
    qint64 WriteData(QByteArray &ba);
    void Finish();

private:
    hid_device *HidDevice;
    bool AboutToFinish;
};

class EUsbHid : public EAbstractProtocomChannel
{
    Q_OBJECT
public:
    explicit EUsbHid(QObject *parent = 0);
    ~EUsbHid();

    bool FirstRun;
    bool Connected;

    bool Connect();

signals:
    void Disconnected();
    void StartUThread(QThread::Priority);
    void StopUThread();

public slots:
    void Disconnect();

private slots:

private:
    EUsbThread *UThread;
    QThread *UThr;

    void ClosePort();
};

#endif // EUSBHID_H
