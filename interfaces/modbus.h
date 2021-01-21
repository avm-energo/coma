#ifndef MODBUS_H
#define MODBUS_H

#include "../gen/datamanager.h"
#include "../gen/logclass.h"
#include "baseinterface.h"
#include "modbusprivate.h"
#include "settingstypes.h"

#include <QMutex>
#include <QTimer>

namespace CommandsMBS
{
using namespace Queries;
// map to translate real commands like "erase memory block" into
// iec104 commands: 45 or 50 or something else
const QMap<Queries::Commands, CommandStruct> CommandsTranslateMap {
    //    map[Queries::QC_SetNewConfiguration] = { CM104_COM45, SetNewConfigurationReg, 0, {} };
    { QC_ClearStartupValues, { MBS_WRITEMULTIPLEREGISTERS, ClearStartupValuesReg, 2, { 0x01, 0x01 } } }, //
    { QC_WriteUserValues, { MBS_WRITEMULTIPLEREGISTERS, 0, 0, {} } },                                    //
    { QC_ReqAlarms, { MBS_READCOILS, 0, 0, {} } },                                                       //
    //    map[Queries::QC_Command50] = { CM104_COM50, 0, 0, {} };
    //    map[Queries::QC_EraseJournals] = { CM104_COM45, EraseJournalsReg, 0, {} };
    { QC_SetStartupValues, { MBS_WRITEMULTIPLEREGISTERS, SetStartupValuesReg, 2, { 0x01, 0x01 } } } //
    //    map[Queries::QC_StartFirmwareUpgrade] = { CM104_COM45, StartFirmwareUpgradeReg, 0, {} };
    //    map[Queries::QC_StartWorkingChannel] = { CM104_COM45, StartWorkingChannelReg, 0, {} };
};
}

class ModBus : public BaseInterface
{
    Q_OBJECT

public:
    //    struct InOutStruct
    //    {
    //        int Command;
    //        QByteArray Ba;
    //        int TaskNum;
    //        Error::Msg Res;
    //        qint64 ReadSize;
    //        bool Checked;
    //    };

    //        typedef struct
    //        {
    //            float flVal;
    //            int SigAdr;
    //        } SignalStruct;

    //        typedef struct
    //        {
    //            quint32 Val;
    //            int SigAdr;
    //        } BSISignalStruct;

    //    struct Information
    //    {
    //        quint16 adr;
    //        int size;
    //    };

    //    struct ComInfo
    //    {
    //        char Command;
    //        quint16 Address;
    //        quint16 Quantity;
    //        quint8 SizeBytes;
    //        QByteArray Data;
    //    };

    //        struct Coils
    //        {
    //            int countBytes;
    //            QByteArray Bytes;
    //        };

    ModBus(QObject *parent = nullptr);
    ~ModBus();

    bool start(const ConnectStruct &st) override;
    void pause() override {};
    void resume() override {};
    //    void BSIrequest();
    void reqStartup(quint32 sigAdr, quint32 sigCount) override;
    void reqBSI() override;
    void reqFile(quint32 filenum, bool isConfigFile) override;
    //    void reqAlarms(quint32 sigAdr, quint32 sigCount);
    void writeFile(quint32 filenum, const QByteArray &file) override;
    //    void writeConfigFile(S2DataTypes::S2ConfigType *) override {};
    void reqTime() override;
    void writeTime(quint32 time) override;
    // writeCommand writes only float signals whose addresses are the lowest and are sequentally lays in the list
    void writeCommand(Queries::Commands cmd, QVariant item) override;
    void writeCommand(Queries::Commands cmd, const QVariantList &list) override;
    void reqFloats(quint32 sigAdr, quint32 sigCount) override;
    //    void reqBitStrings(quint32 sigAdr, quint32 sigCount);

    //    int CheckIndex, CheckHarmIndex, CheckVibrIndex, CorIndex, TimeIndex;
    InterfaceSettings parseSettings(QDomElement domElement) const override;
public slots:
    //    Error::Msg SendAndGetResult(Queries::CommandMBS &request, ModBus::InOutStruct &outp);
    //    void ModWriteCor(ModBus::Information info, float *); //, int*);
    //    void ModReadCor(ModBus::Information info);
    //    void ReadTime();
    //    void WriteTime(uint);
    //    void Tabs(int);
    //    void StartPolling();
    //    void StopPolling();
    // void stop() override;

signals:
    //    void SignalsReceived(QList<ModBus::SignalStruct> Signal);
    //    void CorSignalsReceived(QList<ModBus::SignalStruct> Signal);
    //    void CorSignalsWritten();
    //    void TimeSignalsReceived(QList<ModBus::BSISignalStruct> Signal);
    //    void TimeWritten();
    //    void BsiFromModbus(QList<ModBus::BSISignalStruct>, unsigned int);
    //    // void ModbusState(ConnectionStates);
    //    void ErrorRead();
    //    void ErrorCrc();
    // void Finished();
    // void FinishModbusThread();
    //    void CoilSignalsReady(ModBus::Coils);
    //    void TimeReadError();
    // void ReconnectSignal();

private:
    SerialPortSettings Settings;
    //    int CycleGroup;
    //    QTimer *PollingTimer;
    //    bool TimePollEnabled, MainPollEnabled;
    // bool AboutToFinish;
    //    QByteArray SignalGroups[SIGNALGROUPSNUM];
    //    int _taskCounter;
    //    QQueue<InOutStruct> InQueue;
    //    QList<InOutStruct> OutList;
    // LogClass *Log;

    //    Error::Msg SendAndGet(InOutStruct &inp, InOutStruct &outp);
    //    bool GetResultFromOutQueue(int index, InOutStruct &outp);
    //    Error::Msg GetSignalsFromByteArray(
    //        QByteArray &bain, int startadr, QList<BSISignalStruct> &BSIsig, unsigned int &size);
    //    Error::Msg GetFloatSignalsFromByteArray(QByteArray &bain, int startadr, QList<SignalStruct> &Sig, int &size);
    bool isValidRegs(const CommandsMBS::CommandStruct &cmd) const;
    bool isValidRegs(const quint32 sigAdr, const quint32 sigCount) const;
    CommandsMBS::TypeId type(const quint32 addr, const quint32 count) const;
    CommandsMBS::TypeId type(const quint32 addr, const quint32 count, const CommandsMBS::Commands cmd) const;
    void writeFloat(const DataTypes::FloatStruct &flstr);

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

private slots:
    //    void Polling();
    void SendReconnectSignal();

protected:
};

#endif
