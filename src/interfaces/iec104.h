#ifndef IEC104_H
#define IEC104_H
//#include <gen/datamanager/datamanager.h>
#include "baseinterface.h"
#include "iec104private.h"

#include <QMutex>
#include <QQueue>
#include <QTcpSocket>
#include <QTimer>
#include <gen/logclass.h>

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

    bool start(const ConnectStruct &st) override;
    // InterfaceSettings parseSettings(QDomElement domElement) const override;
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
    void ShowError(QString);
    void Finished();

private:
    bool EthThreadWorking, ParseThreadWorking, AboutToFinish;
    QTcpSocket *sock;

private slots:
    //  void EthThreadStarted();
    void EthThreadFinished();
    void ParseThreadFinished();
    void EmitReconnectSignal();
    void EthStateChanged(QAbstractSocket::SocketState state);
};

#endif // IEC104_H
