#pragma once

#include "baseinterface.h"
#include "interfacesettings.h"

struct UsbHidSettings;

using namespace Interface;

class Protocom final : public BaseInterface
{
    Q_OBJECT

public:
    explicit Protocom(QObject *parent = nullptr);
    ~Protocom();
    bool start(const ConnectStruct &st) override;
    bool start(const UsbHidSettings &usbhid);

    //    void reqTime() override;
    //    void reqFile(quint32 filenum, FileFormat format) override;
    //    void reqStartup(quint32 sigAdr, quint32 sigCount) override;
    //    void reqBSI() override;
    //    void reqBSIExt() override;
    //    void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0) override;
    //    // Support only S2 format
    //    void writeFile(quint32 filenum, const QByteArray &file) override;
    //    void writeTime(quint32 time) override;
    //    void writeCommand(Queries::Commands cmd, QVariant item = 0) override;
    //    void writeCommand(Queries::Commands cmd, const QVariantList &list) override;
    //    void reqFloats(quint32 sigAdr, quint32 sigCount) override;

private:
    Proto::Commands getProtoCommand(Queries::Commands cmd);
    [[deprecated("Use getWSCommand")]] Proto::WCommands getWCommand(Queries::Commands cmd);
    Proto::WSCommands getWSCommand(Queries::Commands cmd);

signals:
    void wakeUpParser() const;
    void wakeUpPort();
};
