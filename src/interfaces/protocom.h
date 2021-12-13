#pragma once

#include "baseinterface.h"
#include "protocomprivate.h"
struct UsbHidSettings;
class ProtocomPrivate;
class Protocom final : public BaseInterface
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Protocom);

public:
    explicit Protocom(QObject *parent = nullptr);
    ~Protocom();
    bool start(const ConnectStruct &st) override;
    bool start(const UsbHidSettings &usbhid);

    void reqTime() override;
    void reqFile(quint32 filenum, FileFormat format) override;
    void reqStartup(quint32 sigAdr, quint32 sigCount) override;
    void reqBSI() override;
    void reqBSIExt() override;
    void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0) override;
    // Support only S2 format
    void writeFile(quint32 filenum, const QByteArray &file) override;
    void writeTime(quint32 time) override;
#ifdef Q_OS_LINUX
    void writeTime(const timespec &time) const;
#endif
    void writeCommand(Queries::Commands cmd, QVariant item = 0) override;
    void writeCommand(Queries::Commands cmd, const QVariantList &list) override;
    void reqFloats(quint32 sigAdr, quint32 sigCount) override;
    void writeRaw(const QByteArray &ba) override;
    InterfaceSettings parseSettings(QDomElement domElement) const override;

protected:
    ProtocomPrivate *const d_ptr;
    Protocom(ProtocomPrivate &dd, QObject *parent);

private:
    bool isValidRegs(const quint32 sigAdr, const quint32 sigCount);
signals:
    void wakeUpParser() const;
    void wakeUpPort();
};

namespace
{

inline const QMap<Queries::Commands, Proto::Commands> getProtoCommand {

    { Queries::Commands::QC_StartFirmwareUpgrade, Proto::Commands::WriteUpgrade }, //
    { Queries::QC_SetNewConfiguration, Proto::Commands::WriteBlkTech },            //
    { Queries::QC_WriteUserValues, Proto::Commands::WriteBlkData },                //
    { Queries::QC_EraseJournals, Proto::Commands::EraseTech },                     //
    { Queries::QC_ReqBitStrings, Proto::Commands::ReadProgress },                  //
    { Queries::QC_EraseTechBlock, Proto::Commands::EraseTech },                    //
    { Queries::QC_Test, Proto::Commands::Test },                                   //
    { Queries::QC_WriteSingleCommand, Proto::Commands::WriteSingleCommand },       //
    { Queries::QUSB_ReqTuningCoef, Proto::Commands::ReadBlkAC },                   //
    { Queries::QUSB_WriteTuningCoef, Proto::Commands::WriteBlkAC },                //
    { Queries::QUSB_ReqBlkData, Proto::Commands::ReadBlkData },                    //
    { Queries::QUSB_ReqBlkDataA, Proto::Commands::ReadBlkDataA },                  //
    { Queries::QUSB_ReqBlkDataTech, Proto::Commands::ReadBlkTech },                //
    { Queries::QUSB_ReqOscInfo, Proto::Commands::ReadBlkTech },                    //
    { Queries::QUSB_WriteBlkDataTech, Proto::Commands::WriteBlkTech },             //
    { Queries::QUSB_Reboot, Proto::Commands::WriteBlkCmd },                        //
    { Queries::QC_ReqAlarms, Proto::Commands::FakeReadAlarms },                    //
    { Queries::QUSB_GetMode, Proto::Commands::ReadMode },                          //
    { Queries::QUSB_SetMode, Proto::Commands::WriteMode },                         //
    { Queries::QUSB_WriteHardware, Proto::Commands::WriteHardware }                //
};

inline const QMap<Queries::Commands, Proto::WCommands> getWCommand {

    { Queries::QC_SetStartupValues, Proto::WCommands::InitStartupValues },    //
    { Queries::QC_ClearStartupValues, Proto::WCommands::EraseStartupValues }, //

};

}
