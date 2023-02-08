#ifndef MODBUSTHREAD_H
#define MODBUSTHREAD_H

#include "modbusprivate.h"

#include <QTimer>
#include <gen/datatypes.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>

class ModbusThread : public QObject
{
    Q_OBJECT
public:
    explicit ModbusThread(QObject *parent = nullptr);
    ~ModbusThread();
    void setDeviceAddress(quint8 adr);
    void setDelay(quint8 newDelay);

public slots:
    void run();
    void finishThread();
    void parseReply(QByteArray ba);

signals:
    void clearBuffer();
    void finished();
    void write(QByteArray);

private:
    bool busy;            ///< Port is busy with write/read operation
    quint8 deviceAddress; ///< Deivce address
    quint8 delay;         ///< Delay in ms

    UniquePointer<LogClass> log;
    QByteArray m_readData;
    CommandsMBS::CommandStruct m_commandSent;
    int m_bytesToReceive;
    QTimer *trashTimer;
    bool mTrashEnabled;

    void readRegisters(CommandsMBS::CommandStruct &cms);
    void readCoils(CommandsMBS::CommandStruct &cms);
    void writeMultipleRegisters(CommandsMBS::CommandStruct &cms);
    void setQueryStartBytes(CommandsMBS::CommandStruct &cms, QByteArray &ba);
    QByteArray createReadPDU(const CommandsMBS::CommandStruct &cms) const;
    QByteArray createADU(const QByteArray &pdu) const;
    void calcCRCAndSend(QByteArray &ba);
    void send(const QByteArray &ba);
    void waitReply();
    void parseAndSetToOutList(QByteArray &ba);
    void getFloatSignals(QByteArray &bain);
    void getIntegerSignals(QByteArray &bain);
    void getCommandResponse(QByteArray &bain);
    void getSinglePointSignals(QByteArray &bain);
    quint16 calcCRC(QByteArray &ba) const;

    template <typename T> T unpackReg(QByteArray ba) const
    {
        assert(sizeof(T) == ba.size());
        for (auto i = 0; i < ba.size(); i = i + 2)
            std::swap(ba.data()[i], ba.data()[i + 1]);
        return *reinterpret_cast<T *>(ba.data());
    }

private slots:
    void trashTimerTimeout();
};

#endif // MODBUSTHREAD_H
