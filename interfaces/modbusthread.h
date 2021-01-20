#ifndef MODBUSTHREAD_H
#define MODBUSTHREAD_H

#include "../gen/datatypes.h"
#include "../gen/logclass.h"
#include "modbusprivate.h"

#define RECONNECTTIME 1000

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
    void readCoils(CommandsMBS::CommandStruct &cms);
    //    void readHoldingRegisters(CommandsMBS::CommandStruct &cms);
    void writeMultipleRegisters(CommandsMBS::CommandStruct &cms);
    void setQueryStartBytes(CommandsMBS::CommandStruct &cms, QByteArray &ba);
    QByteArray createReadPDU(const CommandsMBS::CommandStruct &cms) const;
    QByteArray createADU(const QByteArray &pdu) const;
    void send(QByteArray &ba);
    void sendWithoutCrc(const QByteArray &ba);
    void parseAndSetToOutList(QByteArray &ba);
    void getFloatSignals(QByteArray &bain);
    void getIntegerSignals(QByteArray &bain);
    void getCommandResponse(QByteArray &bain);
    void getSinglePointSignals(QByteArray &bain);
    bool checkReceivedByteArray(QByteArray &bain);

    //    void SendAndGetResult(ModBus::InOutStruct &inp);
    void Send();
    quint16 CalcCRC(QByteArray &Dat) const;

    template <typename T> T unpackReg(QByteArray ba) const
    {
        assert(sizeof(T) == ba.size());
        for (auto i = 0; i < ba.size(); i = i + 2)
            std::swap(ba.data()[i], ba.data()[i + 1]);

        return *reinterpret_cast<T *>(ba.data());
    }

    //    void AddToOutQueue(ModBus::InOutStruct &outp);

signals:
};

#endif // MODBUSTHREAD_H
