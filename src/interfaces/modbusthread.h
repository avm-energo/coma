#ifndef MODBUSTHREAD_H
#define MODBUSTHREAD_H

#include "baseinterfacethread.h"
#include "modbusprivate.h"

#include <QTimer>
#include <gen/datatypes.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>

namespace Interface
{

class ModbusThread : public BaseInterfaceThread
{
    Q_OBJECT
public:
    explicit ModbusThread(QObject *parent = nullptr);
    ~ModbusThread();
    void parseRequest(const CommandStruct &cmdStr) override;
    void parseResponse() override;

    void setDeviceAddress(quint8 adr);
    void setDelay(quint8 newDelay);

public slots:
    void processReadBytes(QByteArray ba) override;

signals:
    void clearBuffer();

private:
    bool m_busy;            ///< Port is busy with write/read operation
    quint8 m_deviceAddress; ///< Deivce address
    quint8 m_delay;         ///< Delay in ms

    quint16 m_fileSectionNum; // текущая считываемая / передаваемая секция файла

    MBS::CommandStruct m_commandSent;
    int m_bytesToReceive;
    QByteArray m_fileData;

    // commands to send
    void readRegisters(MBS::CommandStruct &cms);
    void readCoils(MBS::CommandStruct &cms);
    void writeMultipleRegisters(MBS::CommandStruct &cms);
    bool writeFile(quint16 fileNum);

    // prepare start bytes for data to send
    void setQueryStartBytes(MBS::CommandStruct &cms, QByteArray &ba);

    QByteArray createReadPDU(const MBS::CommandStruct &cms) const;
    QByteArray createADU(const QByteArray &pdu) const;
    void calcCRCAndSend(QByteArray &ba);
    void send(const QByteArray &ba);
    void waitReply();
    void processFloatSignals();
    void processIntegerSignals();
    void processCommandResponse();
    void processSinglePointSignals();
    bool processReadFile();
    quint16 calcCRC(QByteArray &ba) const;

    template <typename T> T unpackReg(QByteArray ba) const
    {
        assert(sizeof(T) == ba.size());
        for (auto i = 0; i < ba.size(); i = i + 2)
            std::swap(ba.data()[i], ba.data()[i + 1]);
        return *reinterpret_cast<T *>(ba.data());
    }

    template <typename T> QByteArray packReg(T value)
    {
        QByteArray ba;
        std::array<char, sizeof(T)> valueBytes;
        memcpy(&valueBytes, &value, sizeof(T));
        for (auto it = valueBytes.begin(); it != valueBytes.end(); it = it + 2)
        {
            ba.push_back(*(it + 1));
            ba.push_back(*it);
        }
        return ba;
    }
};

}
#endif // MODBUSTHREAD_H
