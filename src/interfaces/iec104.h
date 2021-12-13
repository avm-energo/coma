#ifndef IEC104_H
#define IEC104_H
#include "../gen/datamanager.h"
#include "../gen/logclass.h"
#include "baseinterface.h"
#include "iec104private.h"

#include <QMutex>
#include <QQueue>
#include <QTcpSocket>
#include <QTimer>
namespace Commands104
{
using namespace Queries;
// map to translate real commands like "erase memory block" into
// iec104 commands: 45 or 50 or something else
const QMap<Queries::Commands, CommandStruct> CommandsTranslateMap {
    { QC_SetNewConfiguration, { CM104_COM45, SetNewConfigurationReg, 0, {} } }, //
    { QC_ClearStartupValues, { CM104_COM45, ClearStartupValuesReg, 0, {} } },   //
    { QC_WriteUserValues, { CM104_COM50, 0, 0, {} } },                          //
    //{ QC_EraseJournals, { CM104_COM45, EraseJournalsReg, 0, {} } },               //
    { QC_SetStartupValues, { CM104_COM45, SetStartupValuesReg, 0, {} } },         //
    { QC_StartFirmwareUpgrade, { CM104_COM45, StartFirmwareUpgradeReg, 0, {} } }, //
    { QC_StartWorkingChannel, { CM104_COM45, StartWorkingChannelReg, 0, {} } }    //

};
}

class IEC104 final : public BaseInterface
{
    Q_OBJECT

public:
    IEC104(QObject *parent = nullptr);
    ~IEC104();

    //    IEC104Thread *m_thread104;
    //    QList<QByteArray> ReadData;
    //    IEC104Thread::FlSignals104 *flSignals;
    //    static QQueue<QVariant> s_104InputQueue;

    //    struct Settings
    //    {
    //        quint16 baseadr;
    //        QString ip;
    //    };

    //    typedef struct
    //    {
    //        quint8 typeIdent;
    //        quint8 qualifier;
    //        quint16 cause;
    //        quint8 commonAdrASDU;
    //    } DataUnitIdentifier;

    //    typedef struct
    //    {
    //        DataUnitIdentifier Ident;
    //        quint8 AdrObj;      // адрес объекта
    //        quint16 FileName;   // имя файла
    //        quint8 SectionName; // имя секции
    //        quint8 SegmentSize; // длина сегмента
    //        QByteArray Brray;   // Данные

    //    } F_SG;

    //    typedef struct
    //    {
    //        DataUnitIdentifier Ident;
    //        quint8 AdrObj;      // адрес объекта
    //        quint16 FileName;   // имя файла
    //        quint8 SectionName; // имя секции
    //        qint8 AFQ;          // Описатель подтверждения приёма

    //    } F_AF;

    //    typedef struct
    //    {
    //        DataUnitIdentifier Ident;
    //        quint8 AdrObj;      // адрес объекта
    //        quint16 FileName;   // имя файла
    //        quint8 SectionName; // имя секции
    //        qint8 LSQ;          // Описатель последней секции, сегмента
    //        qint8 CHS;          // Контрольная сумма

    //    } F_LS;

    //    typedef struct
    //    {
    //        DataUnitIdentifier Ident;
    //        quint8 AdrObj;      // адрес объекта
    //        quint16 FileName;   // имя файла
    //        quint8 SectionName; // имя секции
    //        qint8 SCQ; // Описатель выбора (первые 4 бита) и вызова (вторые 4 бита)

    //    } F_SC;

    //    F_SC SC;

    //    typedef struct
    //    {
    //        DataUnitIdentifier Ident;
    //        quint8 AdrObj;         // адрес объекта
    //        quint16 FileName;      // имя файла
    //        quint8 SectionName;    // имя секции
    //        quint8 SectionSize[3]; // размер секции
    //        quint8 SRQ;            // указатель готовности секции

    //    } F_SR;

    //    typedef struct
    //    {
    //        DataUnitIdentifier Ident;
    //        quint8 AdrObj;    // адрес объекта
    //        quint16 FileName; // имя файла
    //        quint32 FileSize; // размер файла
    //        quint8 FRQ;       // указатель готовности файла

    //    } F_FR;

    //    QString IP;

    //    bool Working();
    bool start(const ConnectStruct &st) override;
    //    static void getSignalsFrom104(quint32 firstSignalAdr, quint32 signalCount, DataManager::SignalTypes type,
    //        QList<DataManager::SignalsStruct> &outlist);
    InterfaceSettings parseSettings(QDomElement domElement) const override;
public slots:
    void reqStartup(quint32 sigAdr = 0, quint32 sigCount = 0) override;
    void reqBSI() override;
    void reqBSIExt() override;
    // void reqAlarms(quint32 sigAdr = 0, quint32 sigCount = 0);
    void reqFile(quint32 filenum, FileFormat format) override;
    void writeFile(quint32 filenum, const QByteArray &file) override;
    // void writeConfigFile(S2DataTypes::S2ConfigType *s2config) override;
    void reqTime() override;
    void writeTime(quint32 time) override;
    void writeCommand(Queries::Commands cmd, QVariant item) override;
    void reqFloats(quint32 sigAdr = 0, quint32 sigCount = 0) override;
    void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0) override;

    //    static void SelectFile(char);
    void stop() override;

    //    static void Com45(quint32 com);
    //    static void Com50(quint32 adr, float param);
    //    void CorReadRequest();
    //    static void FileReady(S2ConfigType *s2config);
    //    static void getTime();
    //    static void com51WriteTime(uint time);
    bool supportBSIExt() override
    {
        // no way to check
        return true;
    }
signals:
    void StopAll();
    //    void Floatsignalsready(IEC104Thread::FlSignals104 *);
    //    void Sponsignalsready(IEC104Thread::SponSignals *);
    //    void Bs104signalsready(IEC104Thread::BS104Signals *);
    void ShowError(QString);
    //    void SendS2fromiec104(S2ConfigType *);
    //    void SendJourSysfromiec104(QByteArray);
    //    void SendJourWorkfromiec104(QByteArray);
    //    void SendJourMeasfromiec104(QByteArray);
    //    void SendMessageOk();
    //    void SetDataSize(int);
    //    void SetDataCount(int);
    //    void SendConfMessageOk();
    // void ReconnectSignal();
    void Finished();

private:
    bool EthThreadWorking, ParseThreadWorking, AboutToFinish;
    QTcpSocket *sock;
    //    LogClass *Log;
    //    QQueue<IEC104Thread::InputStruct> m_inputQueue;
    //    QList<IEC104Thread::SignalsStruct> m_outputList;
    //    S2ConfigType *S2Config;

private slots:
    // void SelectFile(char);

    //  void EthThreadStarted();
    void EthThreadFinished();
    // void ParseThreadStarted();
    void ParseThreadFinished();
    void EmitReconnectSignal();
    void EthStateChanged(QAbstractSocket::SocketState state);
};

#endif // IEC104_H
