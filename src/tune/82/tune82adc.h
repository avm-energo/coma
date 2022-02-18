#ifndef TUNE82ADC_H
#define TUNE82ADC_H

#include "../../datablocks/82/bac.h"
#include "../../datablocks/bd0.h"
#include "../../datablocks/82/bda.h"
//#include "../../datablocks/82/bdain.h"
#include "../abstracttunedialog.h"

#include <QWidget>

class Tune82ADC : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune82ADC(ConfigV *config, int tuneStep, QWidget *parent = nullptr);

private:
    Bac *m_bac;
    Bda *m_bda;
    //    BdaIn *m_bdain;
    Bd0 *m_bd0;
    double m_pt100;
    //    bool m_isEnergoMonitorDialogCreated;
    int m_curTuneStep;
    //    BdaIn::BlockData m_bdainBlockData;
    int m_BacWidgetIndex, m_BdainWidgetIndex, m_Bd0WidgetIndex;
    float IUefNat_filt_old[6];      // для сохранения значений по п. 7.3.2

    struct MidTuneStruct
    {
        float fet;
        float uet;
        float iet;
        float yet;
    };

    MidTuneStruct m_midTuneStruct;

    void setMessages() override;
    void setTuneFunctions() override;

    Error::Msg setDefBac();
    Error::Msg getAnalogData();
    Error::Msg saveUeff();
    Error::Msg calcPhaseCorrection();
    Error::Msg showPreWarning();

private slots:
};

#endif // TUNE82ADC_H
