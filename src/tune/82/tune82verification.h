#pragma once

#include "../abstracttunedialog.h"

class Bd182;
class Mip;

struct RetomSettings
{
    float phiLoad; // phi
    float voltage; // U
    float current; // I
};

class Tune82Verification : public AbstractTuneDialog
{
private:
    std::map<QString, QString> m_reportData;
    Bd182 *m_bd1;
    Mip *m_mip;
    Modules::MezzanineBoard m_typeM;

    void showRetomDialog(const RetomSettings &retomData);
    void cancelTune();

    void setCurrentsTo(float value);
    void writeReportData(const QString &name, const QString &value);
    void init();
    Error::Msg verification();

public:
    Tune82Verification(S2::Configuration &config, Modules::MezzanineBoard type, //
        int tuneStep, QWidget *parent = nullptr);

    void setTuneFunctions() override;
};
