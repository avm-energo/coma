#ifndef TUNEKIVADC_H
#define TUNEKIVADC_H

#include "../../config/configkiv.h"
#include "../abstracttunedialog.h"
//#include "tunekiv.h"
#include "../../datablocks/kiv/bac.h"
#include "../../datablocks/kiv/bda.h"
#include "../../datablocks/kiv/bdain.h"
#include "../../datablocks/bd0.h"

#include <QWidget>

class TuneKIVADC : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVADC(int tuneStep, ConfigKIV *ckiv, QWidget *parent = nullptr);

private:
//    TuneKIV *TKIV;
    ConfigKIV *CKIV;
    Bac *m_bac;
    Bda *m_bda;
    BdaIn *m_bdain;
    Bd0 *m_bd0;
    double m_pt100;
    bool m_isEnergoMonitorDialogCreated;
    int m_curTuneStep;
    BdaIn::BlockData m_bdainBlockData;
    int m_BacWidgetIndex, m_BdainWidgetIndex;
    //    TuneKIV::Bac m_bac;

    void setMessages() override;
    void setTuneFunctions() override;
    //    void FillBac(int bacnum);
    //    void FillBackBac(int bacnum);
    //    QWidget *MainUI() override;

    Error::Msg showPreWarning();
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
    //    void showTWTab(int num);
    QWidget *BdWidget();
    bool checkBdaIn();
    Error::Msg showEnergomonitorInputDialog();
    //    void saveIntermediateResults();

private slots:
    void CalcTuneCoefs();
    //    void setDefCoefs() override;
};

#endif // TUNEKIVADC_H
