#include "tunekiv.h"

#include "../../models/datadelegate.h"
#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>

TuneKIV::TuneKIV(int bacnum, S2ConfigType *S2Config, QObject *parent) : TuneClass(bacnum, S2Config, parent)
{
    for (int i = 0; i < 3; i++)
    {
        m_Bac_newblock.KmU[i] = 0;
        m_Bac_newblock.KmI1[i] = 0;
        m_Bac_newblock.KmI2[i] = 0;
        m_Bac_newblock.KmI4[i] = 0;
        m_Bac_newblock.KmI8[i] = 0;
        m_Bac_newblock.KmI16[i] = 0;
        m_Bac_newblock.KmI32[i] = 0;
        m_Bac_newblock.DPsi[i] = 0;
        m_Bac_newblock.DPsi[i + 3] = 0;
    }
}

QWidget *TuneKIV::BacWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    ETableView *tv = new ETableView;
    DataDelegate *chdg = new DataDelegate;
    tv->setItemDelegate(chdg);
    for (int i = 0; i < 3; i++)
    {
        m_VModel->setData(m_VModel->index(0, i * 2), "N1_TT[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(0, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(0, i * 2 + 1), &m_Bac.N1_TT[i]);
        m_VModel->setData(m_VModel->index(0, i * 2 + 6), "KmU[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(0, i * 2 + 7), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(0, i * 2 + 7), &m_Bac.KmU[i]);
        m_VModel->setData(m_VModel->index(1, i * 2), "KmI1[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(1, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(1, i * 2 + 1), &m_Bac.KmI1[i]);
        m_VModel->setData(m_VModel->index(1, i * 2 + 6), "KmI2[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(1, i * 2 + 7), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(1, i * 2 + 7), &m_Bac.KmI2[i]);
        m_VModel->setData(m_VModel->index(2, i * 2), "KmI4[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(2, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(2, i * 2 + 1), &m_Bac.KmI4[i]);
        m_VModel->setData(m_VModel->index(2, i * 2 + 6), "KmI8[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(2, i * 2 + 7), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(2, i * 2 + 7), &m_Bac.KmI8[i]);
        m_VModel->setData(m_VModel->index(3, i * 2), "KmI16[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(3, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(3, i * 2 + 1), &m_Bac.KmI16[i]);
        m_VModel->setData(m_VModel->index(3, i * 2 + 6), "KmI32[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(3, i * 2 + 7), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(3, i * 2 + 7), &m_Bac.KmI32[i]);
        m_VModel->setData(m_VModel->index(4, i * 2), "TKPsi_a[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(4, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(4, i * 2 + 1), &m_Bac.TKPsi_a[i]);
        m_VModel->setData(m_VModel->index(4, i * 2 + 6), "TKPsi_b[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(4, i * 2 + 7), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(4, i * 2 + 7), &m_Bac.TKPsi_b[i]);
    }

    for (int i = 0; i < 6; i++)
    {
        m_VModel->setData(m_VModel->index(5, i * 2), "DPsi[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(5, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(5, i * 2 + 1), &m_Bac.DPsi[i]);
        m_VModel->setData(m_VModel->index(6, i * 2), "TKUa[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(6, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(6, i * 2 + 1), &m_Bac.TKUa[i]);
        m_VModel->setData(m_VModel->index(7, i * 2), "TKUb[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(7, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(7, i * 2 + 1), &m_Bac.TKUb[i]);
    }
    m_VModel->setData(m_VModel->index(7, 0), "K_freq");
    m_VModel->setData(m_VModel->index(7, 1), ValueItem::OUTVALUEINT);
    m_VModel->setValueData(m_VModel->index(7, 1), &m_Bac.K_freq);
    m_VModel->setData(m_VModel->index(7, 2), "Art");
    m_VModel->setData(m_VModel->index(7, 3), ValueItem::OUTVALUEINT);
    m_VModel->setValueData(m_VModel->index(7, 3), &m_Bac.Art);
    m_VModel->setData(m_VModel->index(7, 4), "Brt");
    m_VModel->setData(m_VModel->index(7, 5), ValueItem::OUTVALUEINT);
    m_VModel->setValueData(m_VModel->index(7, 5), &m_Bac.Brt);
    m_VModel->setData(m_VModel->index(7, 6), "Brt");
    m_VModel->setData(m_VModel->index(7, 7), ValueItem::OUTVALUEINT);
    m_VModel->setValueData(m_VModel->index(7, 7), &m_Bac.Brt);
    m_VModel->setData(m_VModel->index(7, 8), "Tmk0");
    m_VModel->setData(m_VModel->index(7, 9), ValueItem::OUTVALUEINT);
    m_VModel->setValueData(m_VModel->index(7, 9), &m_Bac.Tmk0);
    tv->setModel(m_VModel);
    vlyout->addWidget(tv);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *TuneKIV::BdaWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Данные без регулировки");
    ETableView *tv = new ETableView;
    DataDelegate *chdg = new DataDelegate;
    tv->setItemDelegate(chdg);
    for (int i = 0; i < 6; i++)
    {
        m_VModel->setData(m_VModel->index(0, i * 2), "Ueff_ADC[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(0, i * 2 + 1), ValueItem::OUTVALUEINT);
        m_VModel->setValueData(m_VModel->index(0, i * 2 + 1), &m_Bda.Ueff_ADC[i]);
    }
    m_VModel->setData(m_VModel->index(1, 0), "Frequency");
    m_VModel->setData(m_VModel->index(1, 1), ValueItem::OUTVALUEINT);
    m_VModel->setValueData(m_VModel->index(1, 1), &m_Bda.Frequency);
    m_VModel->setData(m_VModel->index(1, 2), "Pt100");
    m_VModel->setData(m_VModel->index(1, 3), ValueItem::OUTVALUEINT);
    m_VModel->setValueData(m_VModel->index(1, 3), &m_Bda.Pt100);
    tv->setModel(m_VModel);
    vlyout->addWidget(tv);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

void TuneKIV::SetDefCoefs()
{
    m_Bac.Art = 44.65f;
    m_Bac.Brt = 3345.75f;
    m_Bac.Tmk0 = 24;
    m_Bac.K_freq = 1;

    for (int i = 0; i < 6; i++)
    {
        m_Bac.DPsi[i] = 0;
        m_Bac.TKUa[i] = 0;
        m_Bac.TKUb[i] = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        m_Bac.KmU[i] = 1;
        m_Bac.KmI1[i] = 1;
        m_Bac.KmI16[i] = 1;
        m_Bac.KmI2[i] = 1;
        m_Bac.KmI32[i] = 1;
        m_Bac.KmI8[i] = 1;
        m_Bac.N1_TT[i] = 10;
        m_Bac.TKPsi_a[i] = 0;
        m_Bac.TKPsi_b[i] = 0;
    }
    m_Bac.KmI4[0] = 0.997060814f;
    m_Bac.KmI4[1] = 0.993458061f;
    m_Bac.KmI4[2] = 0.992731500f;

    update();
}
