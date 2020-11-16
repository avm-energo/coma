#ifndef MODBUSTHREAD_H
#define MODBUSTHREAD_H

#include "../gen/datatypes.h"
#include "../gen/logclass.h"
#include "modbusprivate.h"

#include <QMap>
#include <QObject>
#define RECONNECTTIME 5000

class ModbusThread : public QObject
{
    Q_OBJECT
public:
    explicit ModbusThread(QObject *parent = nullptr);
    ~ModbusThread();
    void setDeviceAddress(quint8 adr);

    // ConnectionStates State();
    //    void Init(QQueue<ModBus::InOutStruct> *inq, QList<ModBus::InOutStruct> *outl);

public slots:
    void Run();
    void FinishThread();
    void ParseReply(QByteArray ba);

signals:
    // void ModbusState(ConnectionStates);
    void Finished();
    void Write(QByteArray);

private:
    //    QQueue<ModBus::InOutStruct> *InQueue;
    //    QList<ModBus::InOutStruct> *OutList;
    bool Busy; // port is busy with write/read operation
    bool AboutToFinish;
    quint8 deviceAddress;
    //    ModBus::InOutStruct Inp, Outp;
    // ConnectionStates _state;
    LogClass *Log;
    QByteArray m_readData;
    CommandsMBS::CommandStruct m_commandSent;
    int m_bytesToReceive;

    void readRegisters(CommandsMBS::CommandStruct &cms);
    //    void readHoldingRegisters(CommandsMBS::CommandStruct &cms);
    void writeMultipleRegisters(CommandsMBS::CommandStruct &cms);
    void setQueryStartBytes(CommandsMBS::CommandStruct &cms, QByteArray &ba);
    void send(QByteArray &ba);
    void parseAndSetToOutList(QByteArray &ba);
    void getFloatSignals(QByteArray &bain);
    void getIntegerSignals(QByteArray &bain);
    void getCommandResponse(QByteArray &bain);
    void getSinglePointSignals(QByteArray &bain);
    bool checkReceivedByteArray(QByteArray &bain);

    //    void SendAndGetResult(ModBus::InOutStruct &inp);
    void Send();
    quint16 CalcCRC(QByteArray &Dat);
    //    void AddToOutQueue(ModBus::InOutStruct &outp);

signals:
};

#endif // MODBUSTHREAD_H
