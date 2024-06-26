#pragma once

#include <datablocks/bd0.h>
#include <datablocks/kiv/bac.h>
#include <datablocks/kiv/bac2.h>
#include <datablocks/kiv/bda.h>
#include <datablocks/kiv/bdain.h>
#include <tune/abstracttunedialog.h>

class Tune84ADC final : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune84ADC(int tuneStep, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    BacA284 *m_bac;
    Bac2A284 *m_bac2;
    BdaA284 *m_bda;
    BdaIn *m_bdain;
    Bd0 *m_bd0;
    double m_pt100;
    bool m_isEnergoMonitorDialogCreated;
    int m_curTuneStep;
    BdaIn::BlockData m_bdainBlockData;
    int m_BacWidgetIndex, m_BdainWidgetIndex, m_Bd0WidgetIndex;
    struct MidTuneStruct
    {
        float fet;
        float uet;
        float iet;
        float yet;
    };

    MidTuneStruct m_midTuneStruct;

    void setTuneFunctions() override;

    Error::Msg showPreWarning();
    Error::Msg checkTuneCoefs();
    Error::Msg setSMode2();
    Error::Msg ADCCoef1();
    Error::Msg ADCCoef2();
    Error::Msg ADCCoef4();
    Error::Msg ADCCoef8();
    Error::Msg ADCCoef16();
    Error::Msg ADCCoef32();
    Error::Msg Tmk0();
    Error::Msg SendBac();
    Error::Msg CheckTune();

    Error::Msg ADCCoef(int coef);
    Error::Msg setADCCoef(int coef);
    Error::Msg showRetomDialog(int coef);
    QWidget *BdWidget();
    bool checkBdaIn(int current);
    Error::Msg showEnergomonitorInputDialog();

private slots:
    void CalcTuneCoefs();
};
