    #ifndef IEC104_H
#define IEC104_H
#include <QTcpSocket>
#include "../gen/s2.h"
#include "../gen/logclass.h"

#define BASEADR104          205
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
#define C_SE_NC_1                               50
#define C_BO_NA_1				51	// Bitstring of 32 bit
#define C_SC_TA_1				58	// Single command with time tag CP56Time2a
#define C_DC_TA_1				59	// Double command with time tag CP56Time2a
#define M_EI_NA_1				70	// End of initialization
#define C_IC_NA_1				100	// Interrrogation command
#define C_CI_NA_1				101	// Counter interrrogation command
#define C_CS_NA_1				103	// Clock syncronization command
#define C_TS_NB_1				104	// Test command
#define C_RP_NC_1				105	// Reset process command

#define F_FR_NA_1				120	// File ready
#define F_SR_NA_1				121	// Section ready
#define F_SC_NA_1				122	// Call file
#define F_LS_NA_1				123	// Last section
#define F_AF_NA_1				124	// Confirm receive
#define F_SG_NA_1				125	// Segment of the section
#define F_DR_TA_1				126	// Directory

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


//#define CN_MAXFILESIZE  30000 // максимальный размер выходного файла
#define SECTIONSIZE 2048
#define SEGMENTSIZE 230
#define OFFSET 13
#define FHSIZE 16
#define SYSTEM_JOUR_SIZE 65568


class Parse104 : public QObject
{
    Q_OBJECT

public:
    Parse104(QObject *parent=nullptr);
    ~Parse104();

    typedef struct
    {
        quint8 SigAdr[3];
        quint32 SigVal;
        quint8 SigQuality;
        //quint64 CP56Time;
    }BS104;  // первое - номера сигналов, второе - их значения ("" ~ недостоверное значение), третье - метка времени

    typedef struct
    {
        quint32 SigAdr;
        float SigVal;
        quint8 SigQuality;
        quint64 CP56Time;
    }FLOAT104;

    typedef struct
    {
        quint32 SigAdr;
        quint8 SigVal;
       // quint64 CP56Time;
    }SIQ104;

    typedef struct
    {
        quint32 SigAdr;
        quint8 SigVal;
        quint64 CP56Time;
    }SIQ104withTime;

    typedef struct
    {
       FLOAT104 fl;
       int SigNumber;
    }FlSignals104;

/*    typedef struct
    {
       SIQ104 Spon[256];
       int SigNumber;
    }SponSignals104;

    typedef struct
    {
       SIQ104withTime Spon[256];
       int SigNumber;
    }SponSignalsWithTime; */

    typedef struct
    {
       SIQ104withTime Spon[256];
       int SigNumber;
    } SponSignals;


    typedef struct
    {
       BS104 BS;
       int SigNumber;
    }BS104Signals;



    QByteArray File;
    quint8 firstSegment;
    int Pos;
    QList<QByteArray> ParseData;
    quint32 ReadDataSize;
    quint16 V_S, V_R, AckVR;
    int cmd;
    bool GetNewVR, NewDataArrived;
    QMutex ParseMutex, SignalsMutex;
    QTimer *Timer104;
    QTimer *InterogateTimer;
    QByteArray WriteData;
    char *outdata;
    QByteArray ReadData, ReadDataChunk;
    QTimer *TTimer, *OscTimer;
    quint16 OscNum;
    quint8 bStep;
    int fnum;
    quint8 ReadDataChunkLength; // длина всей посылки
    quint8 RDSize; // длина всей посылки
    int RDLength;
    int outdatasize; // размер приёмной области памяти
    int SegLeft; // количество оставшихся сегментов
    int SegEnd; // номер последнего байта в ReadData текущего сегмента
    bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо
    QVector<S2::DataRec> *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    QVector<S2::DataRec> *DRJour;
    char BoardType;
    quint32 FileLen;
    int incLS, count, NoAnswer, FileSending;
    QTimer *TimerCon;




public slots:
    void Stop();
    void Run();
    void ErrMsg();

signals:
    void Started();
    void Finished();
    void floatsignalsreceived(Parse104::FlSignals104*);
    void sponsignalsreceived(Parse104::SponSignals*);
//    void sponsignalsreceived(Parse104::SponSignals104*);
//    void sponsignalWithTimereceived(Parse104::SponSignalsWithTime*);
    void bs104signalsreceived(Parse104::BS104Signals*);
    void error(int);
    void sendS();
    void parsestarted();
    void GeneralInter();
    void sendAct();
    void CallFile(unsigned char);
    void CallSection(unsigned char);
    void SendConfirmSection(unsigned char);
    void sendConfirmFile(unsigned char);
    void sendS2fromParse(QVector<S2::DataRec>*);
/*    void sendJourSysfromParse(QVector<S2::DataRec>*);
    void sendJourWorkfromParse(QVector<S2::DataRec>*);
    void sendJourMeasfromParse(QVector<S2::DataRec>*); */
    void sendJourSysfromParse(QByteArray);
    void sendJourWorkfromParse(QByteArray);
    void sendJourMeasfromParse(QByteArray);
    void sectionReady();
    void segmentReady();
    void LastSec();
    void sendMessageOk();
    void sendConfMessageOk();
//    void UpdateReleWidget(Parse104::SponSignals104*);
    void SetDataSizeFromParse(int);
    void SetDataCountFromParse(int);
    void sendMessagefromParse();
//    void writeCorMesOkParse();
    void startConTimer();


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

    void ParseIFormat(QByteArray &ba);
    int isIncomeDataValid(QByteArray);

private slots:

protected:
};

class IEC104 : public QObject
{
    Q_OBJECT

public:
    IEC104(QObject *parent = nullptr);
    ~IEC104();

    enum EthernetStates
    {
        Connected,
        Connecting,
        Disconnected
    };

    bool State();

    typedef struct
    {
        quint8 start;
        quint8 APDUlength;
        quint8 contrfield[4];
    } APCI;

    typedef QByteArray ASDU;
    Parse104 *Parse;
    QList<QByteArray> ReadData;
    Parse104::FlSignals104* flSignals;
    quint16 BaseAdr;


    static quint8 stopincrementing;

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
       quint8 AdrObj;              // адрес объекта
       quint16 FileName;           // имя файла
       quint8 SectionName;         // имя секции
       quint8 SegmentSize;         // длина сегмента
       QByteArray Brray;           // Данные

    }F_SG;

    typedef struct
    {
       DataUnitIdentifier Ident;
       quint8 AdrObj;              // адрес объекта
       quint16 FileName;           // имя файла
       quint8 SectionName;         // имя секции
       qint8 AFQ;                  // Описатель подтверждения приёма

    }F_AF;

    typedef struct
    {
       DataUnitIdentifier Ident;
       quint8 AdrObj;              // адрес объекта
       quint16 FileName;           // имя файла
       quint8 SectionName;         // имя секции
       qint8 LSQ;                  // Описатель последней секции, сегмента
       qint8 CHS;                  // Контрольная сумма

    }F_LS;

    typedef struct
    {
       DataUnitIdentifier Ident;
       quint8 AdrObj;              // адрес объекта
       quint16 FileName;           // имя файла
       quint8 SectionName;         // имя секции
       qint8 SCQ;                  // Описатель выбора (первые 4 бита) и вызова (вторые 4 бита)

    }F_SC;

    F_SC SC;

    typedef struct
    {
       DataUnitIdentifier Ident;
       quint8 AdrObj;              // адрес объекта
       quint16 FileName;           // имя файла
       quint8 SectionName;         // имя секции
       quint8 SectionSize[3];        // размер секции
       quint8 SRQ;                 // указатель готовности секции

    }F_SR;

    typedef struct
    {
       DataUnitIdentifier Ident;
       quint8 AdrObj;              // адрес объекта
       quint16 FileName;           // имя файла
       quint32 FileSize;           // размер файла
       quint8 FRQ;                 // указатель готовности файла

    }F_FR;

    quint8 SecNum;
    //static quint8 stopincrementing;
    quint8 KSS;
    quint8 KSF;
    QString IP;

    bool Working();

public slots:
    void Send(int inc, APCI, ASDU=QByteArray());
    void Connect(const QString &IP);
    void Start();
    void Stop();
    void StartConTimer();
    //void ErrMsg();

signals:
    void writedatatoeth(QByteArray);
    void StopAll();
    void error(int);
    void EthConnected();
    void EthDisconnected();
    //void ethNoconnection();
    void floatsignalsready(Parse104::FlSignals104*);
    void sponsignalsready(Parse104::SponSignals*);
//    void sponsignalsready(Parse104::SponSignals104*);
//    void sponsignalWithTimereceived(Parse104::SponSignalsWithTime*);
    void bs104signalsready(Parse104::BS104Signals*);
    void readbytessignal(QByteArray);
    void writebytessignal(QByteArray);
    void ShowError(QString);
    void readConfEth();
    void sendS2fromiec104(QVector<S2::DataRec>*);
/*    void sendJourSysfromiec104(QVector<S2::DataRec>*);
    void sendJourWorkfromiec104(QVector<S2::DataRec>*);
    void sendJourMeasfromiec104(QVector<S2::DataRec>*); */
    void sendJourSysfromiec104(QByteArray);
    void sendJourWorkfromiec104(QByteArray);
    void sendJourMeasfromiec104(QByteArray);
    void LastSeg();
    void sendMessageOk();
//    void relesignalsready(Parse104::SponSignals104*);
    void SetDataSize(int);
    void SetDataCount(int);
    void sendConfMessageOk();
    void sendCorMesOk();
    void ReconnectSignal();
    void Finished();

private:
    QTimer *TTimer, *ConTimer;
    bool GSD;
    QByteArray cutpckt;
    int _state;
    bool EthThreadWorking, ParseThreadWorking;
    LogClass *Log;

    void ParseSomeData(QByteArray); //, bool);

private slots:
    void SendI();
    void SendS();
    void SendTestAct();
    void SendTestCon();
    void GetSomeData(QByteArray);
    void SelectFile(char);
    void CallFile(unsigned char);
    void GetSection(unsigned char);
    void ConfirmSection(unsigned char);
    void ConfirmFile(unsigned char);
    void FileReady(QVector<S2::DataRec>*);
    void SectionReady();
    void SendSegments();
    void LastSegment();
    void LastSection();
    void Com45(quint32 com);
    void Com50(quint32 *adr, float *param);
    void CorReadRequest();
    void InterrogateTimeGr15();
    void com51WriteTime(uint);
    void EthThreadFinished();
    void ParseThreadStarted();
    void ParseThreadFinished();
};

#endif // IEC104_H
