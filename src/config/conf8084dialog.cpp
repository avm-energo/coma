#include "conf8084dialog.h"

#include "configkiv.h"

#include <QGroupBox>
#include <QTabWidget>
#include <QVBoxLayout>
Conf8084Dialog::Conf8084Dialog(ConfigKIV *ckiv, QWidget *parent) : ConfKIVDialog(ckiv, parent)
{
    m_list = {
        CType,
        Abs_104,
        Cycle_104,
        T1_104,
        T2_104,
        T3_104,
        k_104,
        w_104,
        IP_ID,
        Mask_ID,
        GW_ID,
        Port_ID,
        SNTP_ID,
        Baud_ID,
        Parity_ID,
        stopbit_ID,
        adrMB_ID,
        NFiltr_ID,
        Unom1,
        DUImin_ID,
        Imin,
        dC_pred,
        dC_alarm,
        dTg_pred,
        dTg_alarm,
        dIunb_pred,
        dIunb_alarm,
        GdC,
        GdTg,
        GdIunb,
        T_Data_Rec,
        LowU,
        IsU,
        IsIuIunb,
        Tevent_pred,
        Tevent_alarm,
        Tg_Pasp_ID,
        C_Pasp_ID,
        U2nom,
    };
}

// void Conf8084Dialog::SetupUI()
//{
//    QVBoxLayout *lyout = new QVBoxLayout;
//    QTabWidget *ConfTW = new QTabWidget;
//    ConfTW->setObjectName("conftw");

//    ConfTW->addTab(analogWidget(), "Аналоговые");

//    ConfTW->addTab(thresholdsWidget(), "Уставки");

//    ConfTW->addTab(remainsWidget(), "Остальное");

//    ConfTW->addTab(connectionWidget(), "Связь");

//    lyout->addWidget(ConfTW);
//    lyout->addWidget(ConfButtons());
//    setLayout(lyout);
//}

// void Conf8084Dialog::Fill()
//{
//    CKIV->MainConfig()->Fill();
//    FillKiv();
//}

// void Conf8084Dialog::FillBack() const
//{
//    CKIV->MainConfig()->FillBack();
//    FillBackKiv();
//}

// QWidget *Conf8084Dialog::connectionWidget()
//{
//    QWidget *w = new QWidget;

//    QVBoxLayout *lyout = new QVBoxLayout;
//    QVBoxLayout *vlyout = new QVBoxLayout;
//    QGroupBox *gb = new QGroupBox;

//    QHBoxLayout *hlyout = new QHBoxLayout;

//    gb->setTitle("Настройки протокола МЭК-60870-5-104");

//    hlyout->addWidget(CKIV->MainConfig()->MainWidget(this));

//    gb->setLayout(hlyout);
//    lyout->addWidget(gb);

//    gb = new QGroupBox("Настройка времени");

//    vlyout->addWidget(CKIV->MainConfig()->TimeWidget(this));

//    gb->setLayout(vlyout);
//    lyout->addWidget(gb);

//    w->setLayout(lyout);
//    return w;
//}
