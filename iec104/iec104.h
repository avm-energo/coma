#ifndef IEC104_H
#define IEC104_H
#include "../gen/logclass.h"
#include "../gen/s2.h"
#include "iec104thread.h"

#include <QMutex>
#include <QQueue>
#include <QTimer>

class IEC104 : public QObject
{
    Q_OBJECT

public:
    IEC104(S2ConfigType *s2, QObject *parent = nullptr);
    ~IEC104();

    //    IEC104Thread *m_thread104;
    //    QList<QByteArray> ReadData;
    //    IEC104Thread::FlSignals104 *flSignals;
    //    static QQueue<QVariant> s_104InputQueue;

    struct Settings
    {
        quint16 baseadr;
        QString ip;
    };

    typedef struct
    {
        quint8 typeIdent;
        quint8 qualifier;
        quint16 cause;
        quint8 commonAdrASDU;
    } DataUnitIdentifier;

    typedef struct
    {
        DataUnitIdentifier Ident;
        quint8 AdrObj;      // адрес объекта
        quint16 FileName;   // имя файла
        quint8 SectionName; // имя секции
        quint8 SegmentSize; // длина сегмента
        QByteArray Brray;   // Данные

    } F_SG;

    typedef struct
    {
        DataUnitIdentifier Ident;
        quint8 AdrObj;      // адрес объекта
        quint16 FileName;   // имя файла
        quint8 SectionName; // имя секции
        qint8 AFQ;          // Описатель подтверждения приёма

    } F_AF;

    typedef struct
    {
        DataUnitIdentifier Ident;
        quint8 AdrObj;      // адрес объекта
        quint16 FileName;   // имя файла
        quint8 SectionName; // имя секции
        qint8 LSQ;          // Описатель последней секции, сегмента
        qint8 CHS;          // Контрольная сумма

    } F_LS;

    typedef struct
    {
        DataUnitIdentifier Ident;
        quint8 AdrObj;      // адрес объекта
        quint16 FileName;   // имя файла
        quint8 SectionName; // имя секции
        qint8 SCQ; // Описатель выбора (первые 4 бита) и вызова (вторые 4 бита)

    } F_SC;

    F_SC SC;

    typedef struct
    {
        DataUnitIdentifier Ident;
        quint8 AdrObj;         // адрес объекта
        quint16 FileName;      // имя файла
        quint8 SectionName;    // имя секции
        quint8 SectionSize[3]; // размер секции
        quint8 SRQ;            // указатель готовности секции

    } F_SR;

    typedef struct
    {
        DataUnitIdentifier Ident;
        quint8 AdrObj;    // адрес объекта
        quint16 FileName; // имя файла
        quint32 FileSize; // размер файла
        quint8 FRQ;       // указатель готовности файла

    } F_FR;

    QString IP;

    bool Working();
    void Connect(Settings &st);
    static void getSignalsFrom104(quint32 firstSignalAdr, quint32 signalCount, IEC104Thread::IEC104SignalTypes type,
        QList<IEC104Thread::SignalsStruct> &outlist);

public slots:
    void SelectFile(char);
    void StopAllThreads();
    void Com45(quint32 com);
    void Com50(quint32 adr, float param);
    void CorReadRequest();
    void FileReady(S2ConfigType *);
    void InterrogateTimeGr15();
    void com51WriteTime(uint time);

signals:
    void StopAll();
    //    void Floatsignalsready(IEC104Thread::FlSignals104 *);
    //    void Sponsignalsready(IEC104Thread::SponSignals *);
    //    void Bs104signalsready(IEC104Thread::BS104Signals *);
    void ShowError(QString);
    void SendS2fromiec104(S2ConfigType *);
    void SendJourSysfromiec104(QByteArray);
    void SendJourWorkfromiec104(QByteArray);
    void SendJourMeasfromiec104(QByteArray);
    void SendMessageOk();
    void SetDataSize(int);
    void SetDataCount(int);
    void SendConfMessageOk();
    void ReconnectSignal();
    void Finished();

private:
    bool EthThreadWorking, ParseThreadWorking, AboutToFinish;
    LogClass *Log;
    QQueue<IEC104Thread::InputStruct> m_inputQueue;
    //    QList<IEC104Thread::SignalsStruct> m_outputList;
    S2ConfigType *S2Config;

private slots:
    // void SelectFile(char);

    void EthThreadStarted();
    void EthThreadFinished();
    void ParseThreadStarted();
    void ParseThreadFinished();
    void EmitReconnectSignal();
};

#endif // IEC104_H
