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

protected:
    ProtocomPrivate *const d_ptr;
    Protocom(ProtocomPrivate &dd, QObject *parent);

private:
    bool isValidRegs(const quint32 sigAdr, const quint32 sigCount);
    Proto::Commands getProtoCommand(Queries::Commands cmd);
    [[deprecated("Use getWSCommand")]] Proto::WCommands getWCommand(Queries::Commands cmd);
    Proto::WSCommands getWSCommand(Queries::Commands cmd);

signals:
    void wakeUpParser() const;
    void wakeUpPort();
};
