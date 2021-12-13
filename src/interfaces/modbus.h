#ifndef MODBUS_H
#define MODBUS_H

#include "../gen/datamanager.h"
#include "../gen/logclass.h"
#include "baseinterface.h"
#include "modbusprivate.h"
#include "settingstypes.h"

#include <QMutex>
#include <QTimer>
#include <array>

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

class ModBus final : public BaseInterface
{
    Q_OBJECT

public:
    ModBus(QObject *parent = nullptr);
    ~ModBus();

    bool start(const ConnectStruct &connectStruct) override;
    void pause() override {};
    void resume() override {};

    void reqStartup(quint32 sigAdr, quint32 sigCount) override;
    void reqBSI() override;
    void reqBSIExt() override;
    void reqFile(quint32 filenum, FileFormat format) override;

    void writeFile(quint32 filenum, const QByteArray &file) override;

    void reqTime() override;
    void writeTime(quint32 time) override;
    // writeCommand writes only float signals whose addresses are the lowest and are sequentally lays in the list
    void writeCommand(Queries::Commands cmd, QVariant item) override;
    void writeCommand(Queries::Commands cmd, const QVariantList &list) override;
    void reqFloats(quint32 sigAdr, quint32 sigCount) override;
    void reqBitStrings(quint32 sigAdr, quint32 sigCount) override {};

    InterfaceSettings parseSettings(QDomElement domElement) const override;

signals:
    void clearBuffer();

private:
    bool isValidRegs(const CommandsMBS::CommandStruct &cmd) const;
    bool isValidRegs(const quint32 sigAdr, const quint32 sigCount) const;
    CommandsMBS::TypeId type(const quint32 addr, const quint32 count) const;
    CommandsMBS::TypeId type(const quint32 addr, const quint32 count, const CommandsMBS::Commands cmd) const;
    void writeFloat(const DataTypes::FloatStruct &flstr);

    quint8 obtainDelay(quint32 baudRate) const
    {
        switch (baudRate)
        {
        case 2400:
            return 16;
        case 4800:
            return 8;
        case 9600:
            return 4;
        case 19200:
            return 3;
        default:
            return 2;
        }
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

private slots:

    void sendReconnectSignal();

protected:
};

#endif
