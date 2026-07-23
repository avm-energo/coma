#pragma once

#include <datablocks/84/bac.h>
#include <datablocks/84/bac2.h>
#include <datablocks/84/bda.h>
#include <datablocks/84/bdain.h>
#include <datablocks/bd0.h>
#include <tune/abstracttunedialog.h>
#include <tune/tunetypes.h>

class Tune84ADC final : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune84ADC(TuneTypes tuneType, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    Bac284 *m_bac;
    Bac84 *m_bac2;
    Bda84 *m_bda;
    BdaIn84 *m_bdain;
    Bd0 *m_bd0;
    double m_pt100;
    bool m_isEnergoMonitorDialogCreated;
    int m_curTuneStep;
    BdaIn84::BlockData m_bdainBlockData;
    int m_BacWidgetIndex, m_BdainWidgetIndex, m_Bd0WidgetIndex;
    struct MidTuneStruct
    {
        float fet;
        float uet;
        float iet;
        float yet;
    };

    MidTuneStruct m_midTuneStruct;
    TuneTypes m_tuneType;

    void setTuneFunctions() override;

    Error::Msg showPreWarning();
    Error::Msg checkTuneCoefs();
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
