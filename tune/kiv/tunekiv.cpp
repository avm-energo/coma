#include "tunekiv.h"

//#include "../../models/valuedelegate.h"
#include "../../widgets/wd_func.h"
#include "../gen/colors.h"
#include "../gen/stdfunc.h"

#include <QGroupBox>
#include <QVBoxLayout>

// TuneKIV::TuneKIV(int bacnum, S2ConfigType *S2Config, QObject *parent) : TuneClass(bacnum, S2Config, parent)
TuneKIV::TuneKIV()
{
    //    for (int i = 0; i < 3; i++)
    //    {
    //        m_Bac_newblock.KmU[i] = 0;
    //        m_Bac_newblock.KmI1[i] = 0;
    //        m_Bac_newblock.KmI2[i] = 0;
    //        m_Bac_newblock.KmI4[i] = 0;
    //        m_Bac_newblock.KmI8[i] = 0;
    //        m_Bac_newblock.KmI16[i] = 0;
    //        m_Bac_newblock.KmI32[i] = 0;
    //        m_Bac_newblock.DPsi[i] = 0;
    //        m_Bac_newblock.DPsi[i + 3] = 0;
    //    }
    m_BdaWidgetIsSet = m_BacWidgetIsSet = m_Bd0WidgetIsSet = false;
    SetDefCoefs();
}

TuneKIV::Bac TuneKIV::defBacBlock()
{
    return m_defBacBlock;
}

QWidget *TuneKIV::BacWidget()
{
    if (m_BacWidgetIsSet)
        return m_BacWidget;
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                             "background-color: "
        + QString(Colors::AConfO) + "; font: bold 10px;}";
    m_BacWidget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    //    ETableView *tv = new ETableView;
    //    ValueDelegate *chdg = new ValueDelegate;
    //    tv->setItemDelegate(chdg);
    //    for (int i = 0; i < 3; i++)
    //    {
    //        m_VModel->setData(m_VModel->index(0, i * 2), "N1_TT[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(0, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(0, i * 2 + 1), &m_Bac.N1_TT[i]);
    //        m_VModel->setData(m_VModel->index(0, i * 2 + 6), "KmU[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(0, i * 2 + 7), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(0, i * 2 + 7), &m_Bac.KmU[i]);
    //        m_VModel->setData(m_VModel->index(1, i * 2), "KmI1[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(1, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(1, i * 2 + 1), &m_Bac.KmI1[i]);
    //        m_VModel->setData(m_VModel->index(1, i * 2 + 6), "KmI2[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(1, i * 2 + 7), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(1, i * 2 + 7), &m_Bac.KmI2[i]);
    //        m_VModel->setData(m_VModel->index(2, i * 2), "KmI4[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(2, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(2, i * 2 + 1), &m_Bac.KmI4[i]);
    //        m_VModel->setData(m_VModel->index(2, i * 2 + 6), "KmI8[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(2, i * 2 + 7), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(2, i * 2 + 7), &m_Bac.KmI8[i]);
    //        m_VModel->setData(m_VModel->index(3, i * 2), "KmI16[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(3, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(3, i * 2 + 1), &m_Bac.KmI16[i]);
    //        m_VModel->setData(m_VModel->index(3, i * 2 + 6), "KmI32[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(3, i * 2 + 7), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(3, i * 2 + 7), &m_Bac.KmI32[i]);
    //        m_VModel->setData(m_VModel->index(4, i * 2), "TKPsi_a[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(4, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(4, i * 2 + 1), &m_Bac.TKPsi_a[i]);
    //        m_VModel->setData(m_VModel->index(4, i * 2 + 6), "TKPsi_b[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(4, i * 2 + 7), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(4, i * 2 + 7), &m_Bac.TKPsi_b[i]);
    //    }

    //    for (int i = 0; i < 6; i++)
    //    {
    //        m_VModel->setData(m_VModel->index(5, i * 2), "DPsi[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(5, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(5, i * 2 + 1), &m_Bac.DPsi[i]);
    //        m_VModel->setData(m_VModel->index(6, i * 2), "TKUa[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(6, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(6, i * 2 + 1), &m_Bac.TKUa[i]);
    //        m_VModel->setData(m_VModel->index(7, i * 2), "TKUb[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(7, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(7, i * 2 + 1), &m_Bac.TKUb[i]);
    //    }
    //    m_VModel->setData(m_VModel->index(7, 0), "K_freq");
    //    m_VModel->setData(m_VModel->index(7, 1), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(7, 1), &m_Bac.K_freq);
    //    m_VModel->setData(m_VModel->index(7, 2), "Art");
    //    m_VModel->setData(m_VModel->index(7, 3), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(7, 3), &m_Bac.Art);
    //    m_VModel->setData(m_VModel->index(7, 4), "Brt");
    //    m_VModel->setData(m_VModel->index(7, 5), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(7, 5), &m_Bac.Brt);
    //    m_VModel->setData(m_VModel->index(7, 6), "Brt");
    //    m_VModel->setData(m_VModel->index(7, 7), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(7, 7), &m_Bac.Brt);
    //    m_VModel->setData(m_VModel->index(7, 8), "Tmk0");
    //    m_VModel->setData(m_VModel->index(7, 9), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(7, 9), &m_Bac.Tmk0);
    //    tv->setModel(m_VModel);
    //    vlyout->addWidget(tv);
    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "N1_TT[" + QString::number(i) + "]"), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i), "", ValuesLEFormat), 1, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "KmU[" + QString::number(i) + "]"), 0, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 3), "", ValuesLEFormat), 1, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "KmI1[" + QString::number(i) + "]"), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 6), "", ValuesLEFormat), 3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "KmI2[" + QString::number(i) + "]"), 2, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 9), "", ValuesLEFormat), 3, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "KmI4[" + QString::number(i) + "]"), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 12), "", ValuesLEFormat), 5, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "KmI8[" + QString::number(i) + "]"), 4, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 15), "", ValuesLEFormat), 5, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "KmI16[" + QString::number(i) + "]"), 6, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 18), "", ValuesLEFormat), 7, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "KmI32[" + QString::number(i) + "]"), 6, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 21), "", ValuesLEFormat), 7, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "TKPsi_a[" + QString::number(i) + "]"), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 24), "", ValuesLEFormat), 9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "TKPsi_b[" + QString::number(i) + "]"), 8, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 27), "", ValuesLEFormat), 9, i + 3, 1, 1);
    }
    for (int i = 0; i < 6; i++)
    {
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "DPsi[" + QString::number(i) + "]"), 10, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 30), "", ValuesLEFormat), 11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "TKUa[" + QString::number(i) + "]"), 12, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 36), "", ValuesLEFormat), 13, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "TKUb[" + QString::number(i) + "]"), 14, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE(m_BacWidget, "tune" + QString::number(i + 42), "", ValuesLEFormat), 15, i, 1, 1);
    }

    glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "K_freq"), 16, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune48", "", ValuesLEFormat), 17, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "Art"), 16, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune49", "", ValuesLEFormat), 17, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "Brt"), 16, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune50", "", ValuesLEFormat), 17, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(m_BacWidget, "Tmk0"), 16, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLE(m_BacWidget, "tune51", "", ValuesLEFormat), 17, 3, 1, 1);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    m_BacWidget->setLayout(lyout);
    m_BacWidgetIsSet = true;
    return m_BacWidget;
}

QWidget *TuneKIV::BdaWidget()
{
    if (m_BdaWidgetIsSet)
        return m_BdaWidget;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                           "background-color: "
        + QString(Colors::AConfO) + "; font: bold 10px;}";
    m_BdaWidget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    //    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Данные без регулировки");
    for (int i = 0; i < 6; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "Ueff_ADC" + IndexStr), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(m_BdaWidget, "", "bdavalue" + QString::number(i), ValuesFormat,
                              "Ueff_ADC" + IndexStr + ".Измеренные сигналы в кодах АЦП"),
            1, i, 1, 1);
    }

    glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "Frequency"), 2, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(m_BdaWidget, "", "bdavalue6", ValuesFormat, "Частота"), 3, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "Pt100"), 4, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(m_BdaWidget, "", "bdavalue7", ValuesFormat, "Температура"), 5, 0, 1, 1);
    //    ETableView *tv = new ETableView;
    //    bdavalueDelegate *chdg = new bdavalueDelegate;
    //    tv->setItemDelegate(chdg);
    //    for (int i = 0; i < 6; i++)
    //    {
    //        m_VModel->setData(m_VModel->index(0, i * 2), "Ueff_ADC[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(0, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(0, i * 2 + 1), &m_Bda.Ueff_ADC[i]);
    //    }
    //    m_VModel->setData(m_VModel->index(1, 0), "Frequency");
    //    m_VModel->setData(m_VModel->index(1, 1), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(1, 1), &m_Bda.Frequency);
    //    m_VModel->setData(m_VModel->index(1, 2), "Pt100");
    //    m_VModel->setData(m_VModel->index(1, 3), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(1, 3), &m_Bda.Pt100);
    //    tv->setModel(m_VModel);
    //    vlyout->addWidget(tv);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    m_BdaWidget->setLayout(lyout);
    m_BdaWidgetIsSet = true;
    return m_BdaWidget;
}

QWidget *TuneKIV::BdaInWidget()
{
    if (m_Bda_inWidgetIsSet)
        return m_Bda_inWidget;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                           "background-color: "
        + QString(Colors::AConfO) + "; font: bold 10px;}";
    m_Bda_inWidget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    //    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Измеренные данные");
    for (int i = 0; i < 6; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "IUefNat_filt" + IndexStr), 0, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(m_BdaWidget, "", "bdainvalue" + QString::number(i), ValuesFormat,
                              "Истинные действующие значения сигналов в В или мА на входе"),
            1, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "IUeff_filtered" + IndexStr), 2, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(m_BdaWidget, "", "bdainvalue" + QString::number(i + 6), ValuesFormat,
                              "Действующие значения сигналов по 1-й гармонике"),
            3, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "phi_next_f" + IndexStr), 4, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(m_BdaWidget, "", "bdainvalue" + QString::number(i + 12), ValuesFormat,
                              "Углы сдвига сигналов по 1-й гармонике относительно Ua в градусах"),
            5, i, 1, 1);
    }
    for (int i = 0; i < 3; ++i)
    {
        QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "Cbush" + IndexStr), 6, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(m_BdaWidget, "", "bdainvalue" + QString::number(i + 18), ValuesFormat, "Емкости вводов"), 7,
            i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "Tg d" + IndexStr), 6, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLBLT(m_BdaWidget, "", "bdainvalue" + QString::number(i + 21), ValuesFormat, "Tg delta вводов"),
            7, i + 3, 1, 1);
    }
    glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "Frequency"), 8, 0, 1, 3);
    glyout->addWidget(WDFunc::NewLBLT(m_BdaWidget, "", "bdainvalue24", ValuesFormat, "Частота в сети, Гц"), 9, 0, 1, 3);
    glyout->addWidget(WDFunc::NewLBL(m_BdaWidget, "Pt100"), 8, 3, 1, 3);
    glyout->addWidget(
        WDFunc::NewLBLT(m_BdaWidget, "", "bdainvalue25", ValuesFormat, "Измеренное сопротивление термометра, Ом"), 9, 3,
        1, 3);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    m_Bda_inWidget->setLayout(lyout);
    m_Bda_inWidgetIsSet = true;
    return m_Bda_inWidget;
}

QWidget *TuneKIV::Bd0Widget()
{
    if (m_Bd0WidgetIsSet)
        return m_Bd0Widget;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                           "background-color: "
        + QString(Colors::AConfO) + "; font: bold 10px;}";
    m_Bd0Widget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGroupBox *gb = new QGroupBox("Общие данные");
    hlyout->addWidget(WDFunc::NewLBL(m_Bd0Widget, "Tmk0"));
    hlyout->addWidget(WDFunc::NewLBLT(m_Bd0Widget, "", "tmk0", ValuesFormat, "Температура процессора"));
    lyout->addLayout(hlyout);
    hlyout->addWidget(WDFunc::NewLBL(m_Bd0Widget, "VBat"));
    hlyout->addWidget(WDFunc::NewLBLT(m_Bd0Widget, "", "vbat", ValuesFormat, "Напряжение батарейки"));
    lyout->addLayout(hlyout);
    //    ETableView *tv = new ETableView;
    //    bdavalueDelegate *chdg = new bdavalueDelegate;
    //    tv->setItemDelegate(chdg);
    //    for (int i = 0; i < 6; i++)
    //    {
    //        m_VModel->setData(m_VModel->index(0, i * 2), "Ueff_ADC[" + QString::number(i) + "]");
    //        m_VModel->setData(m_VModel->index(0, i * 2 + 1), ValueItem::OUTVALUEINT);
    //        m_VModel->setValueData(m_VModel->index(0, i * 2 + 1), &m_Bda.Ueff_ADC[i]);
    //    }
    //    m_VModel->setData(m_VModel->index(1, 0), "Frequency");
    //    m_VModel->setData(m_VModel->index(1, 1), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(1, 1), &m_Bda.Frequency);
    //    m_VModel->setData(m_VModel->index(1, 2), "Pt100");
    //    m_VModel->setData(m_VModel->index(1, 3), ValueItem::OUTVALUEINT);
    //    m_VModel->setValueData(m_VModel->index(1, 3), &m_Bda.Pt100);
    //    tv->setModel(m_VModel);
    //    vlyout->addWidget(tv);
    gb->setLayout(lyout);
    lyout->addWidget(gb);
    m_Bd0Widget->setLayout(lyout);
    m_Bd0WidgetIsSet = true;
    return m_Bd0Widget;
}

void TuneKIV::SetDefCoefs()
{
    m_defBacBlock.Art = 44.65f;
    m_defBacBlock.Brt = 3345.75f;
    m_defBacBlock.Tmk0 = 24;
    m_defBacBlock.K_freq = 1;

    for (int i = 0; i < 6; i++)
    {
        m_defBacBlock.DPsi[i] = 0;
        m_defBacBlock.TKUa[i] = 0;
        m_defBacBlock.TKUb[i] = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        m_defBacBlock.KmU[i] = 1;
        m_defBacBlock.KmI1[i] = 1;
        m_defBacBlock.KmI16[i] = 1;
        m_defBacBlock.KmI2[i] = 1;
        m_defBacBlock.KmI32[i] = 1;
        m_defBacBlock.KmI8[i] = 1;
        m_defBacBlock.N1_TT[i] = 10;
        m_defBacBlock.TKPsi_a[i] = 0;
        m_defBacBlock.TKPsi_b[i] = 0;
    }
    m_defBacBlock.KmI4[0] = 0.997060814f;
    m_defBacBlock.KmI4[1] = 0.993458061f;
    m_defBacBlock.KmI4[2] = 0.992731500f;
}

void TuneKIV::updateBacWidget()
{
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i), QString::number(m_Bac.N1_TT[i], 'g', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 3), QString::number(m_Bac.KmU[i], 'f', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 6), QString::number(m_Bac.KmI1[i], 'f', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 9), QString::number(m_Bac.KmI2[i], 'f', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 12), QString::number(m_Bac.KmI4[i], 'f', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 15), QString::number(m_Bac.KmI8[i], 'f', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 18), QString::number(m_Bac.KmI16[i], 'f', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 21), QString::number(m_Bac.KmI32[i], 'f', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 24), QString::number(m_Bac.TKPsi_a[i], 'e', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 27), QString::number(m_Bac.TKPsi_b[i], 'e', 5));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 30), QString::number(m_Bac.DPsi[i], 'f', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 36), QString::number(m_Bac.TKUa[i], 'e', 5));
        WDFunc::SetLEData(m_BacWidget, "tune" + QString::number(i + 42), QString::number(m_Bac.TKUb[i], 'e', 5));
    }

    WDFunc::SetLEData(m_BacWidget, "tune48", QString::number(m_Bac.K_freq, 'f', 5));
    WDFunc::SetLEData(m_BacWidget, "tune49", QString::number(m_Bac.Art, 'f', 5));
    WDFunc::SetLEData(m_BacWidget, "tune50", QString::number(m_Bac.Brt, 'f', 5));
    WDFunc::SetLEData(m_BacWidget, "tune51", QString::number(m_Bac.Tmk0, 'f', 5));
}

void TuneKIV::updateBdaWidget()
{
    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLBLText(
            m_BdaWidget, "bdavalue" + QString::number(i), WDFunc::StringValueWithCheck(m_Bda.Ueff_ADC[i], 4));
    }

    WDFunc::SetLBLText(m_BdaWidget, "bdavalue6", WDFunc::StringValueWithCheck(m_Bda.Frequency, 4));
    WDFunc::SetLBLText(m_BdaWidget, "bdavalue7", WDFunc::StringValueWithCheck(m_Bda.Pt100, 4));
}

void TuneKIV::updateBdaInWidget()
{
    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLBLText(m_Bda_inWidget, "bdainvalue" + QString::number(i),
            WDFunc::StringValueWithCheck(m_Bda_in.IUefNat_filt[i], 3));
        WDFunc::SetLBLText(m_Bda_inWidget, "bdainvalue" + QString::number(i + 6),
            WDFunc::StringValueWithCheck(m_Bda_in.IUeff_filtered[i], 3));
        WDFunc::SetLBLText(m_Bda_inWidget, "bdainvalue" + QString::number(i + 12),
            WDFunc::StringValueWithCheck(m_Bda_in.phi_next_f[i], 4));
        if (i < 3)
        {
            WDFunc::SetLBLText(m_Bda_inWidget, "bdainvalue" + QString::number(i + 18),
                WDFunc::StringValueWithCheck(m_Bda_in.Cbush[i], 1));
            WDFunc::SetLBLText(m_Bda_inWidget, "bdainvalue" + QString::number(i + 21),
                WDFunc::StringValueWithCheck(m_Bda_in.Tg_d[i], 4));
        }
    }

    WDFunc::SetLBLText(m_Bda_inWidget, "bdavalue24", WDFunc::StringValueWithCheck(m_Bda_in.Frequency, 4));
    WDFunc::SetLBLText(m_Bda_inWidget, "bdavalue25", WDFunc::StringValueWithCheck(m_Bda_in.Pt100_R, 3));
}

void TuneKIV::updateBd0Widget()
{
    WDFunc::SetLBLText(m_Bd0Widget, "tmk0", WDFunc::StringValueWithCheck(m_Bd0.Tmk, 3));
    WDFunc::SetLBLText(m_Bd0Widget, "vbat", WDFunc::StringValueWithCheck(m_Bd0.Vbat, 3));
}

void TuneKIV::updateFromBacWidget()
{
    QString tmps;
    for (int i = 0; i < 3; i++)
    {
        m_Bac.N1_TT[i] = WDFunc::LEData(m_BacWidget, "tune" + QString::number(i)).toUInt();
        m_Bac.KmU[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 3)));
        m_Bac.KmI1[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 6)));
        m_Bac.KmI2[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 9)));
        m_Bac.KmI4[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 12)));
        m_Bac.KmI8[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 15)));
        m_Bac.KmI16[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 18)));
        m_Bac.KmI32[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 21)));
        m_Bac.TKPsi_a[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 24)));
        m_Bac.TKPsi_b[i] = StdFunc::toFloat(WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 27)));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 30));
        m_Bac.DPsi[i] = StdFunc::toFloat(tmps);
        WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 36));
        m_Bac.TKUa[i] = StdFunc::toFloat(tmps);
        WDFunc::LEData(m_BacWidget, "tune" + QString::number(i + 42));
        m_Bac.TKUb[i] = StdFunc::toFloat(tmps);
    }

    WDFunc::LEData(m_BacWidget, "tune48");
    m_Bac.K_freq = StdFunc::toFloat(tmps);
    WDFunc::LEData(m_BacWidget, "tune49");
    m_Bac.Art = StdFunc::toFloat(tmps);
    WDFunc::LEData(m_BacWidget, "tune50");
    m_Bac.Brt = StdFunc::toFloat(tmps);
    WDFunc::LEData(m_BacWidget, "tune51");
    m_Bac.Tmk0 = StdFunc::toFloat(tmps);
}
