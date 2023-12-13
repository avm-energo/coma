#ifndef IEC104PARSER_H
#define IEC104PARSER_H

#include <QQueue>
#include <QTimer>
#include <QVariant>
#include <QWaitCondition>
#include <gen/datatypes.h>
#include <gen/error.h>
#include <gen/logclass.h>
#include <interfaces/connection.h>
#include <s2/filestruct.h>
#include <s2/s2datatypes.h>

namespace Interface
{

class IEC104Parser : public QObject
{
    Q_OBJECT
public:
    IEC104Parser(RequestQueue &queue, QObject *parent = nullptr);

    void setBaseAdr(quint16 adr);
    void parseRequest([[maybe_unused]] const CommandStruct &cmdStr);
    void parseResponse();

public slots:
    void startDT();
    void stopDT();
    void stop();
    void run();
    void processReadBytes(QByteArray ba);

signals:
    void finished();
    void writeData(QByteArray ba);
    void reconnectSignal();
    void sendMessagefromParse();
    void responseSend(const Interface::DeviceResponse &resp);

private:
    std::reference_wrapper<RequestQueue> m_queue;
    int m_signalCounter, m_noAnswer;
    bool m_isFileSending;
    // flag indicates how file should be restored
    DataTypes::FileFormat m_fileIsConfigFile;
    QByteArray m_file;
    QList<QByteArray> m_parseData;
    quint16 m_V_S, m_V_R, m_ackVR;
    int m_command;
    quint32 m_fileLen;
    QTimer *m_timer104;
    QByteArray m_readData;
    quint8 m_readSize; // длина всей посылки
    int m_readPos;
    bool m_threadMustBeFinished;
    quint8 m_APDULength;
    quint8 m_APDUFormat;
    quint8 m_sectionNum;
    quint8 m_baseAdrHigh, m_baseAdrLow;
    quint16 m_baseAdr;
    QTimer *m_sendTestTimer;
    quint8 m_KSS;
    quint8 m_KSF;
    QByteArray m_tempBuffer;
    bool m_isFirstParse;
    bool m_writingToPortBlocked;
    LogClass m_log;

    void parseIFormat(QByteArray &ba);
    Error::Msg isIncomeDataValid(QByteArray &);
    QByteArray createGI(unsigned char apdulength);
    QByteArray ASDUFilePrefix(Iec104::MessageDataType Command, unsigned char filenum, unsigned char secnum);
    QByteArray ASDU6Prefix(Iec104::MessageDataType Command, quint32 adr);

    template <typename T> QByteArray ToByteArray(T var);

    void send(int inc, QByteArray apci, QByteArray asdu = QByteArray());
    void sendGI();
    void sendS();
    void sendTestCon();
    void selectFile(char numfile);
    void callFile(unsigned char);
    void getSection(unsigned char);
    void confirmSection(unsigned char);
    void confirmFile(unsigned char);
    void fileReady(quint16 numfile);
    void sectionReady();
    void sendSegments();
    void lastSection();
    void com45(quint32 com, bool value);
    void com50(quint32 adr, float param);
    void reqGroup(int groupNum);
    void com51WriteTime(quint32 time);
    void setGeneralResponse(DataTypes::GeneralResponseTypes type, quint64 data = 0);
    bool handleFile(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format);

private slots:
    void SendTestAct();
};

}
#endif // IEC104PARSER_H
