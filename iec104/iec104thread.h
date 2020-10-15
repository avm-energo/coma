#ifndef IEC104THREAD_H
#define IEC104THREAD_H

#include "../gen/logclass.h"
#include "../gen/s2.h"

#include <QObject>
#include <QQueue>
#include <QVariant>

#define BASEADR104 205
#define I104_START 0x68

#define I104_I 0x00
#define I104_S 0x01
#define I104_U 0x03
#define I104_WRONG 0xFF

#define I104_W 8

#define I104_STARTDT_ACT 0x07
#define I104_STARTDT_CON 0x0B
#define I104_STOPDT_ACT 0x13
#define I104_STOPDT_CON 0x23
#define I104_TESTFR_ACT 0x43
#define I104_TESTFR_CON 0x83u

// определения возвращаемого значения функции isIncomeDataValid
#define I104_RCVNORM 0x00
#define I104_RCVSMSIZE 0x01
#define I104_RCVWRONG 0x02

// определения типа данных
// TYPE IDENTIFICATION

#define M_SP_NA_1 1  // Single-point information
#define M_DP_NA_1 3  // Double-point information
#define M_ST_NA_1 5  // Step position information
#define M_BO_NA_1 7  // Bitstring of 32 bit
#define M_ME_NA_1 9  // Measured value, normalized value
#define M_ME_NC_1 13 // Measured value, short floating point value
#define M_SP_TB_1 30 // Single-point information with time tag CP56Time2a
#define M_DP_TB_1 31 // Double-point information with time tag CP56Time2a
#define M_ST_TB_1 32 // Step position information with time tag CP56Time2a
#define M_ME_TD_1 34 // Measured value, normalized value with time tag CP56Time2a
#define M_ME_TF_1 36 // Measured value, short floating point value with time tag CP56Time2a
#define M_IT_TB_1 37 // Integrated totals value with time tag CP56Time2a
#define C_SC_NA_1 45 // Single command
#define C_DC_NA_1 46 // Double command
#define C_RC_NA_1 47 // Regulating step command
#define C_SE_NA_1 48 // Set point command, normalised value
#define C_SE_NC_1 50
#define C_BO_NA_1 51  // Bitstring of 32 bit
#define C_SC_TA_1 58  // Single command with time tag CP56Time2a
#define C_DC_TA_1 59  // Double command with time tag CP56Time2a
#define M_EI_NA_1 70  // End of initialization
#define C_IC_NA_1 100 // Interrrogation command
#define C_CI_NA_1 101 // Counter interrrogation command
#define C_CS_NA_1 103 // Clock syncronization command
#define C_TS_NB_1 104 // Test command
#define C_RP_NC_1 105 // Reset process command

#define F_FR_NA_1 120 // File ready
#define F_SR_NA_1 121 // Section ready
#define F_SC_NA_1 122 // Call file
#define F_LS_NA_1 123 // Last section
#define F_AF_NA_1 124 // Confirm receive
#define F_SG_NA_1 125 // Segment of the section
#define F_DR_TA_1 126 // Directory

// CAUSE OF TRANSMISSION: define causes

#define COT_PERIODIC 1
#define COT_BACKGROUND 2
#define COT_SPONTANEOUS 3
#define COT_INITIALISED 4
#define COT_REQUEST 5
#define COT_ACTIVATION 6
#define COT_ACTIVATIONCONFIRM 7
#define COT_DEACTIVATION 8
#define COT_DEACTIVATIONCONFIRM 9
#define COT_ACTIVATIONTERMINATION 10
#define COT_REMOTECOMMAND 11
#define COT_LOCALCOMMAND 12
#define COT_FILETRANSFER 13
#define COT_INTERROGATION 20

#define SECTIONSIZE 2048
#define SEGMENTSIZE 230
#define OFFSET 13
#define FHSIZE 16
#define SYSTEM_JOUR_SIZE 65568

namespace IEC104Signals
{
struct BitString
{
    //    quint8 sigAdr[3];
    quint32 sigAdr;
    quint32 sigVal;
    quint8 sigQuality;
}; // первое - номера сигналов, второе - их значения ("" ~ недостоверное
   // значение), третье - метка времени

struct FloatWithTime
{
    quint32 sigAdr;
    float sigVal;
    quint8 sigQuality;
    quint64 CP56Time;
};

struct SinglePointWithTime
{
    quint32 sigAdr;
    quint8 sigVal;
    quint64 CP56Time;
};
}

Q_DECLARE_METATYPE(IEC104Signals::BitString)
Q_DECLARE_METATYPE(IEC104Signals::FloatWithTime)
Q_DECLARE_METATYPE(IEC104Signals::SinglePointWithTime)

class IEC104Thread : public QObject
{
    Q_OBJECT
public:
    enum IEC104SignalTypes
    {
        BitString,
        FloatWithTime,
        SinglePointWithTime
    };

    enum CommandsEnum
    {
        CM104_INTERROGATETIMEGR15,
        CM104_COM51WRITETIME,
        CM104_COM45,
        CM104_COM50,
        CM104_CORREADREQUEST,
        CM104_SELECTFILE,
        CM104_FILEREADY
    };

    struct CommandsArgs
    {
        quint32 uintarg;
        float flarg;
        void *ptrarg;
    };

    struct InputStruct
    {
        CommandsEnum cmd;
        CommandsArgs args;
    };

    /*    typedef struct
        {
            FLOAT104 fl;
            int SigNumber;
        } FlSignals104;

        typedef struct
        {
            SIQ104withTime Spon[256];
            int SigNumber;
        } SponSignals;

        typedef struct
        {
            BS104 BS;
            int SigNumber;
        } BS104Signals; */

    struct SignalsStruct
    {
        IEC104SignalTypes type;
        QVariant data;
    };

    QByteArray File;
    QList<QByteArray> ParseData;
    quint32 ReadDataSize;
    quint16 V_S, V_R, AckVR;
    int Command;
    QTimer *Timer104;
    QByteArray ReadData;
    quint8 RDSize; // длина всей посылки
    int RDLength;
    S2ConfigType *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    S2ConfigType *DRJour;
    quint32 FileLen;
    int incLS, count, NoAnswer;
    bool FileSending;
    QQueue<InputStruct> *m_inputQueue;
    static QList<SignalsStruct> m_outputList;
    static QMutex s_ParseReadMutex;
    static QMutex s_ParseWriteMutex;
    static QMutex s_IEC104OutQueueMutex;

    IEC104Thread(LogClass *log, QQueue<InputStruct> &queue, S2ConfigType *s2, QObject *parent = nullptr);
    ~IEC104Thread();

    void SetBaseAdr(quint16 adr);

public slots:
    void StartDT();
    void StopDT();
    void Stop();
    void Run();
    void GetSomeData(QByteArray);

signals:
    void Started();
    void Finished();
    void WriteData(QByteArray);
    void ReconnectSignal();
    //    void Floatsignalsreceived(IEC104Thread::FlSignals104 *);
    //    void Sponsignalsreceived(IEC104Thread::SponSignals *);
    //    void Bs104signalsreceived(IEC104Thread::BS104Signals *);
    void SendS2fromParse(S2ConfigType *);
    void SendJourSysfromParse(QByteArray);
    void SendJourWorkfromParse(QByteArray);
    void SendJourMeasfromParse(QByteArray);
    void SendMessageOk();
    void SetDataSize(int);
    void SetDataCount(int);
    void SendMessagefromParse();

private:
    typedef struct
    {
        unsigned char Number; // number of Informational Objects
        unsigned char SQ;     // Single <0> or Series <1> of Objects
    } QualifierVariableStructute;

    typedef struct
    {
        unsigned char cause;     // <0..63> cause number
        unsigned char confirm;   // <0> - positive , <1> - negative
        unsigned char test;      // <0> - not a test, <1> - test
        unsigned char initiator; // number of initiating address
    } CauseOfTransmission;

    typedef struct
    {
        quint8 typeIdent;
        QualifierVariableStructute qualifier;
        CauseOfTransmission cause;
        quint8 commonAdrASDU;
    } DataUnitIdentifier;

    typedef QByteArray APCI, ASDU;

    bool ThreadMustBeFinished;
    quint8 APDULength;
    quint8 APDUFormat;
    quint8 SecNum;
    LogClass *Log;
    quint16 BaseAdr;
    QTimer *ConTimer;
    quint8 KSS;
    quint8 KSF;
    QByteArray CutPckt;
    bool FirstParse;

    void ParseIFormat(QByteArray &ba);
    Error::Msg isIncomeDataValid(QByteArray);
    QByteArray CreateGI(unsigned char apdulength);
    QByteArray ASDUFilePrefix(unsigned char Command, unsigned char filenum, unsigned char secnum);
    QByteArray ASDU6Prefix(unsigned char Command, quint32 adr);
    template <typename T> QByteArray ToByteArray(T var);
    void Send(int inc, APCI, ASDU = QByteArray());
    void SendGI();
    void SendS();
    void SendTestCon();
    void CorReadRequest();
    void SelectFile(char numfile);
    void CallFile(unsigned char);
    void GetSection(unsigned char);
    void ConfirmSection(unsigned char);
    void ConfirmFile(unsigned char);
    void FileReady();
    void SectionReady();
    void SendSegments();
    void LastSection();
    void Com45(quint32 com);
    void Com50(quint32 adr, float param);
    void InterrogateTimeGr15();
    void Com51WriteTime(quint32 time);
    //    void convert(IEC104Thread::SponSignals *signal);
    template <typename T> void addSignalToQueue(IEC104SignalTypes type, T signal)
    {
        SignalsStruct str;
        str.type = type;
        str.data.setValue(signal);
        s_IEC104OutQueueMutex.lock();
        m_outputList.append(str);
        s_IEC104OutQueueMutex.unlock();
    }

private slots:
    void SendTestAct();

protected:
};

#endif // IEC104THREAD_H
