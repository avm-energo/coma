#ifndef MODBUSPARSER_H
#define MODBUSPARSER_H

#include <QTimer>
#include <gen/datatypes.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>
#include <interfaces/parsers/baseprotocolparser.h>
#include <interfaces/types/modbus_types.h>

namespace Interface
{

class ModbusParser : public BaseProtocolParser
{
    Q_OBJECT
public:
    explicit ModbusParser(RequestQueue &queue, QObject *parent = nullptr);
    ~ModbusParser();
    void parseRequest(const CommandStruct &cmdStr) override;
    void parseResponse() override;

    void setDeviceAddress(quint8 adr);

public slots:
    void processReadBytes(QByteArray ba) override;

private:
    quint8 m_deviceAddress;   ///< Deivce address
    quint16 m_fileSectionNum; // текущая считываемая / передаваемая секция файла
    MBS::CommandStruct m_commandSent;
    int m_bytesToReceive;
    QByteArray m_fileData;
    int m_sentBytesCount;

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
    void processFloatSignals();
    void processIntegerSignals();
    void processCommandResponse();
    void processSinglePointSignals();
    bool processReadFile();

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
        std::copy(&value, &value + sizeof(T), valueBytes.begin());
        for (auto it = valueBytes.begin(); it != valueBytes.end(); it = it + 2)
        {
            ba.push_back(*(it + 1));
            ba.push_back(*it);
        }
        return ba;
    }
};

}
#endif // MODBUSPARSER_H
