#pragma once

#include "../../datablocks/82/bd1.h"
#include "../abstracttunedialog.h"

class Mip;
struct MipDataStruct;
struct RetomSettings;
struct VerificationOffset;

class Tune82Verification final : public AbstractTuneDialog
{
private:
    Bd182 *m_bd1;
    Mip *m_mip;
    Modules::MezzanineBoard m_typeM;

    void showRetomDialog(const RetomSettings &retomData);
    void writeMipDataToReport(const MipDataStruct &mipData, const std::size_t iter);
    void writeDeviceDataToReport(const Bd182::BlockData &deviceData, const std::size_t iter);
    void writeOffsetDataToReport(const VerificationOffset &offset, const std::size_t iter);

    Error::Msg setupNFiltrValue();
    void init();
    Error::Msg verification();

public:
    Tune82Verification(S2::Configuration &config, Modules::MezzanineBoard type, //
        int tuneStep, QWidget *parent = nullptr);

    void setTuneFunctions() override;
};
