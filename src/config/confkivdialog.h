#ifndef CONFDIALOGKIV_H
#define CONFDIALOGKIV_H

#include "abstractconfdialog.h"

#include <set>
class ConfigKIV;
class ConfKIVDialog : public AbstractConfDialog
{

public:
    explicit ConfKIVDialog(ConfigKIV *ckiv, QWidget *parent = nullptr);
    ~ConfKIVDialog();

private:
    void Fill() override;
    void FillBack() const override;
    void SetupUI() override;
    void CheckConf() const override;

protected:
    QWidget *analogWidget();
    QWidget *thresholdsWidget();
    QWidget *remainsWidget();
    virtual QWidget *connectionWidget();
    QWidget *testWidget();

    void FillKiv();
    void FillBackKiv() const;

    ConfigKIV *CKIV;

private:
    const std::set<BciNumber> list { CType, Abs_104, Cycle_104, T1_104, T2_104, T3_104, k_104, w_104, IP_ID, Mask_ID,
        GW_ID, Port_ID, SNTP_ID, Baud_ID, Parity_ID, stopbit_ID, adrMB_ID, NFiltr_ID, Unom1, DUImin_ID, Imin, dC_pred,
        dC_alarm, dTg_pred, dTg_alarm, dIunb_pred, dIunb_alarm, GdC, GdTg, GdIunb, RTerm, W100, T_Data_Rec, LowU, IsU,
        IsIuIunb, Tevent_pred, Tevent_alarm, Tg_Pasp_ID, C_Pasp_ID, U2nom, MBMaster, MBMab1, MBMab2, MBMab3, MBMab4 };
    void SetDefConf() override;
};

#endif // CONFDIALOG84_H
