#pragma once

#include "../../datablocks/82/bac.h"
#include "../../datablocks/82/bd1.h"
#include "../../datablocks/82/bda.h"
#include "../../datablocks/bd0.h"
#include "../abstracttunedialog.h"
#include "../mip.h"

class Tune82ADC : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune82ADC(S2::Configuration &config, Modules::MezzanineBoard type, int tuneStep, QWidget *parent = nullptr);

private:
    Bac82 *m_bac, m_bacNewBlock;
    Bd182 *m_bd1;
    Bda82 *m_bda;
    Bd0 *m_bd0;
    Modules::MezzanineBoard m_typeM;
    int m_curTuneStep;
    int m_BacWidgetIndex, m_BdaWidgetIndex, m_Bd0WidgetIndex;
    float IUefNat_filt_old[6]; // для сохранения значений по п. 7.3.2

    struct MidTuneStruct
    {
        float fet;
        float uet;
        float iet;
        float yet;
    };

    MidTuneStruct m_midTuneStruct;
    Mip::MipDataStruct mipdata;

    void setTuneFunctions() override;

    Error::Msg setDefBac();
    Error::Msg getAnalogData();
    Error::Msg saveUeff();
    Error::Msg calcPhaseCorrection();
    Error::Msg calcInterChannelCorrelation();
    Error::Msg calcIUcoef1();
    Error::Msg calcIcoef5();
    Error::Msg showPreWarning();
    Error::Msg checkTune();

    void setCurrentsTo(float i);
    void getBd1();

private slots:
};
