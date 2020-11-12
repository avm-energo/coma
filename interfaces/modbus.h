#ifndef MODBUS_H
#define MODBUS_H

#include "../gen/datamanager.h"
#include "../gen/logclass.h"
#include "baseinterface.h"
#include "settingstypes.h"

#include <QMutex>
#include <QQueue>
#include <QSerialPort>
#include <QTimer>
#include <QWaitCondition>

#define POLLINGINTERVAL 300 // polling cycle time
#define SIGNALGROUPSNUM 7
#define MAINSLEEPCYCLETIME 50

#define READHOLDINGREGISTERS 0x03
#define READINPUTREGISTER 0x04
#define WRITEMULTIPLEREGISTERS 0x10

//#define INITREG 4000
//#define INITREGCOUNT 11 // 11 floats results in 22 bytes
#define TIMEREG 4600
#define TIMEREGCOUNT 1
#define SETINITREG 900
#define CLEARREG 905

class ModBus : public BaseInterface
{
    Q_OBJECT

public:
    //    struct InOutStruct
    //    {
    //        int Command;
    //        QByteArray Ba;
    //        int TaskNum;
    //        Error::Msg Res;
    //        qint64 ReadSize;
    //        bool Checked;
    //    };

    //        typedef struct
    //        {
    //            float flVal;
    //            int SigAdr;
    //        } SignalStruct;

    //        typedef struct
    //        {
    //            quint32 Val;
    //            int SigAdr;
    //        } BSISignalStruct;

    //    struct Information
    //    {
    //        quint16 adr;
    //        int size;
    //    };

    //    struct ComInfo
    //    {
    //        char Command;
    //        quint16 Address;
    //        quint16 Quantity;
    //        quint8 SizeBytes;
    //        QByteArray Data;
    //    };

    //        struct Coils
    //        {
    //            int countBytes;
    //            QByteArray Bytes;
    //        };

    ModBus(QObject *parent = nullptr);
    ~ModBus();

    bool start(const ConnectStruct &st);
    //    void BSIrequest();

    void reqStartup(quint32 sigAdr, quint32 sigCount);
    void reqBSI();
    void reqFile(quint32 filenum, bool isConfigFile);
    //    void reqAlarms(quint32 sigAdr, quint32 sigCount);
    void writeFile(quint32 filenum, const QByteArray &file);
    void reqTime();
    void writeTime(quint32 time);
    // writeCommand writes only float signals whose addresses are the lowest and are sequentally lays in the list
    void writeCommand(Queries::Commands cmd, QList<DataTypes::SignalsStruct> list = QList<DataTypes::SignalsStruct>());
    void reqFloats(quint32 sigAdr, quint32 sigCount);
    //    void reqBitStrings(quint32 sigAdr, quint32 sigCount);

    //    int CheckIndex, CheckHarmIndex, CheckVibrIndex, CorIndex, TimeIndex;

public slots:
    //    Error::Msg SendAndGetResult(Queries::CommandMBS &request, ModBus::InOutStruct &outp);
    //    void ModWriteCor(ModBus::Information info, float *); //, int*);
    //    void ModReadCor(ModBus::Information info);
    //    void ReadTime();
    //    void WriteTime(uint);
    //    void Tabs(int);
    //    void StartPolling();
    //    void StopPolling();
    void stop();

signals:
    //    void SignalsReceived(QList<ModBus::SignalStruct> Signal);
    //    void CorSignalsReceived(QList<ModBus::SignalStruct> Signal);
    //    void CorSignalsWritten();
    //    void TimeSignalsReceived(QList<ModBus::BSISignalStruct> Signal);
    //    void TimeWritten();
    //    void BsiFromModbus(QList<ModBus::BSISignalStruct>, unsigned int);
    //    // void ModbusState(ConnectionStates);
    //    void ErrorRead();
    //    void ErrorCrc();
    void Finished();
    void FinishModbusThread();
    //    void CoilSignalsReady(ModBus::Coils);
    //    void TimeReadError();
    // void ReconnectSignal();

private:
    SerialPortSettings Settings;
    //    int CycleGroup;
    //    QTimer *PollingTimer;
    //    bool TimePollEnabled, MainPollEnabled;
    bool AboutToFinish;
    //    QByteArray SignalGroups[SIGNALGROUPSNUM];
    //    int _taskCounter;
    //    QQueue<InOutStruct> InQueue;
    //    QList<InOutStruct> OutList;
    // LogClass *Log;

    //    Error::Msg SendAndGet(InOutStruct &inp, InOutStruct &outp);
    //    bool GetResultFromOutQueue(int index, InOutStruct &outp);
    //    Error::Msg GetSignalsFromByteArray(
    //        QByteArray &bain, int startadr, QList<BSISignalStruct> &BSIsig, unsigned int &size);
    //    Error::Msg GetFloatSignalsFromByteArray(QByteArray &bain, int startadr, QList<SignalStruct> &Sig, int &size);

private slots:
    //    void Polling();
    void SendReconnectSignal();

protected:
};

#endif
