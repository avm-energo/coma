#pragma once
#include "baseinterface.h"
#include "settingstypes.h"
class Emulator : public BaseInterface
{
public:
    Emulator(QObject *parent = nullptr);

    // BaseInterface interface
public:
    bool start(const ConnectStruct &st) override;
    void reqStartup(quint32 sigAdr, quint32 sigCount) override;
    void reqBSI() override;
    void reqBSIExt() override;
    void reqFile(quint32, FileFormat format) override;
    void writeFile(quint32, const QByteArray &) override;
    void reqTime() override;
    void writeTime(quint32) override;
    void writeCommand(Queries::Commands, QVariant) override;
    void reqFloats(quint32 sigAdr, quint32 sigCount) override;
    void reqBitStrings(quint32 sigAdr, quint32 sigCount) override;
    InterfaceSettings parseSettings(QDomElement domElement) const override;

private:
    EmulatorSettings m_settings;
};
