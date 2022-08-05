#pragma once

#include "../../datablocks/82/bac.h"
#include "../../datablocks/82/bd1.h"
#include "../../datablocks/bd0.h"
#include "../abstracttunedialog.h"

class Tune82ADC : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune82ADC(ConfigV *config, int tuneStep, QWidget *parent = nullptr);

private:
    Bac82 *m_bac;
    Bd182 *m_bda;
    Bd0 *m_bd0;
    double m_pt100;
    int m_curTuneStep;
    int m_BacWidgetIndex, m_BdainWidgetIndex, m_Bd0WidgetIndex;
    float IUefNat_filt_old[6]; // для сохранения значений по п. 7.3.2

    struct MidTuneStruct
    {
        float fet;
        float uet;
        float iet;
        float yet;
    };

    MidTuneStruct m_midTuneStruct;

    void setTuneFunctions() override;

    Error::Msg setDefBac();
    Error::Msg getAnalogData();
    Error::Msg saveUeff();
    Error::Msg calcPhaseCorrection();
    Error::Msg showPreWarning();

private slots:
};
