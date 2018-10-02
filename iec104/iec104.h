#ifndef IEC104_H
#define IEC104_H

#define I104_START          0x68

#define I104_I              0x00
#define I104_S              0x01
#define I104_U              0x03
#define I104_WRONG          0xFF

#define I104_W              8

#define I104_STARTDT_ACT    0x07
#define I104_STARTDT_CON    0x0B
#define I104_STOPDT_ACT     0x13
#define I104_STOPDT_CON     0x23
#define I104_TESTFR_ACT     0x43
#define I104_TESTFR_CON     0x83

// определения возвращаемого значения функции isIncomeDataValid
#define I104_RCVNORM        0x00
#define I104_RCVSMSIZE      0x01
#define I104_RCVWRONG       0x02

// определения типа данных
//TYPE IDENTIFICATION

#define M_SP_NA_1				1		// Single-point information
#define M_DP_NA_1				3		// Double-point information
#define M_ST_NA_1				5		// Step position information
#define M_BO_NA_1				7		// Bitstring of 32 bit
#define M_ME_NA_1				9		// Measured value, normalized value
#define M_ME_NC_1				13	// Measured value, short floating point value
#define M_SP_TB_1				30	// Single-point information with time tag CP56Time2a
#define M_DP_TB_1				31	// Double-point information with time tag CP56Time2a
#define M_ST_TB_1				32	// Step position information with time tag CP56Time2a
#define M_ME_TD_1				34	// Measured value, normalized value with time tag CP56Time2a
#define M_ME_TF_1				36	// Measured value, short floating point value with time tag CP56Time2a
#define M_IT_TB_1				37	// Integrated totals value with time tag CP56Time2a
#define C_SC_NA_1				45	// Single command
#define C_DC_NA_1				46	// Double command
#define C_RC_NA_1				47	// Regulating step command
#define C_SE_NA_1				48	// Set point command, normalised value
#define C_BO_NA_1				51	// Bitstring of 32 bit
#define C_SC_TA_1				58	// Single command with time tag CP56Time2a
#define C_DC_TA_1				59	// Double command with time tag CP56Time2a
#define M_EI_NA_1				70	// End of initialization
#define C_IC_NA_1				100	// Interrrogation command
#define C_CI_NA_1				101	// Counter interrrogation command
#define C_CS_NA_1				103	// Clock syncronization command
#define C_TS_NB_1				104	// Test command
#define C_RP_NC_1				105	// Reset process command

// CAUSE OF TRANSMISSION: define causes

#define COT_PERIODIC							1
#define COT_BACKGROUND							2
#define COT_SPONTANEOUS							3
#define COT_INITIALISED							4
#define COT_REQUEST								5
#define COT_ACTIVATION							6
#define COT_ACTIVATIONCONFIRM               	7
#define COT_DEACTIVATION						8
#define COT_DEACTIVATIONCONFIRM                 9
#define COT_ACTIVATIONTERMINATION               10
#define COT_REMOTECOMMAND						11
#define COT_LOCALCOMMAND						12
#define COT_FILETRANSFER						13
#define COT_INTERROGATION						20

#include <QObject>
#include <QTimer>
#include <QMutex>

class Parse104 : public QObject
{
    Q_OBJECT

public:
    Parse104(QObject *parent=nullptr);
    ~Parse104();

    typedef struct
    {
        quint32 SigNum;
        float SigVal;
        quint8 SigQuality;
        quint64 CP56Time;
    } Signals104; // первое - номера сигналов, второе - их значения ("" ~ недостоверное значение), третье - метка времени

    QList<QByteArray> ParseData;
    quint32 ReadDataSize;
    quint16 V_S, V_R, AckVR;
    int cmd;
    bool GetNewVR, NewDataArrived;
    QMutex ParseMutex, SignalsMutex;

public slots:
    void Stop();
    void Run();

signals:
    void signalsreceived(Parse104::Signals104 &);
    void error(int);
    void sendS();
    void parsestarted();

private:
    typedef struct
    {
        unsigned char Number; // number of Informational Objects
        unsigned char SQ; // Single <0> or Series <1> of Objects
    } QualifierVariableStructute;

    typedef struct
    {
        unsigned char cause; // <0..63> cause number
        unsigned char confirm; // <0> - positive , <1> - negative
        unsigned char test; // <0> - not a test, <1> - test
        unsigned char initiator; // number of initiating address
    } CauseOfTransmission;

    typedef struct
    {
        quint8 typeIdent;
        QualifierVariableStructute qualifier;
        CauseOfTransmission cause;
        quint8 commonAdrASDU;
    } DataUnitIdentifier;

    DataUnitIdentifier DUI;
    bool ThreadMustBeFinished;
    quint8 APDULength;
    quint8 APDUFormat;

    void ParseIFormat(const char *);
    int isIncomeDataValid(QByteArray);

private slots:

protected:
};

class iec104 : public QObject
{
    Q_OBJECT

public:
    iec104(QObject *parent = nullptr);
    ~iec104();

    typedef struct
    {
        quint8 start;
        quint8 APDUlength;
        quint8 contrfield[4];
    } APCI;

    typedef QByteArray ASDU;
    Parse104 *Parse;
    QList<QByteArray> ReadData;
    QList<Parse104::Signals104> Signals;
    bool ParseStarted;

public slots:
    void Send(APCI, ASDU=QByteArray());
    void Start();
    void Stop();

signals:
    void writedatatoeth(QByteArray);
    void stopall();
    void error(int);
    void ethconnected();
    void ethdisconnected();
    void signalsready(Parse104::Signals104&);

private:
    QTimer *TTimer;
    bool GSD;
    QByteArray cutpckt;
    void ParseSomeData(QByteArray, bool);

private slots:
    void SendS();
    void GetSomeData(QByteArray);
    void StartParse();
};

#endif // IEC104_H
