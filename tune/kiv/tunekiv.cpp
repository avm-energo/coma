#include "tunekiv.h"
#include "../../models/checkdelegate.h"
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
    CheckDelegate *chdg = new CheckDelegate;
    tv->setItemDelegate(chdg);
    for (int i = 0; i < 3; i++)
    {
        m_VModel->setData(m_VModel->index(0, i * 2), "N1_TT[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(0, i * 2 + 1), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(0, i * 2 + 1), &m_Bac_block.N1_TT[i]);
        m_VModel->setData(m_VModel->index(0, i * 2 + 6), "KmU[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(0, i * 2 + 7), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(0, i * 2 + 7), &m_Bac_block.KmU[i]);
        m_VModel->setData(m_VModel->index(1, i * 2), "KmI1[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(1, i * 2 + 1), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(1, i * 2 + 1), &m_Bac_block.KmI1[i]);
        m_VModel->setData(m_VModel->index(1, i * 2 + 6), "KmI2[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(1, i * 2 + 7), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(1, i * 2 + 7), &m_Bac_block.KmI2[i]);
        m_VModel->setData(m_VModel->index(2, i * 2), "KmI4[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(2, i * 2 + 1), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(2, i * 2 + 1), &m_Bac_block.KmI4[i]);
        m_VModel->setData(m_VModel->index(2, i * 2 + 6), "KmI8[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(2, i * 2 + 7), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(2, i * 2 + 7), &m_Bac_block.KmI8[i]);
        m_VModel->setData(m_VModel->index(3, i * 2), "KmI16[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(3, i * 2 + 1), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(3, i * 2 + 1), &m_Bac_block.KmI16[i]);
        m_VModel->setData(m_VModel->index(3, i * 2 + 6), "KmI32[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(3, i * 2 + 7), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(3, i * 2 + 7), &m_Bac_block.KmI32[i]);
        m_VModel->setData(m_VModel->index(4, i * 2), "TKPsi_a[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(4, i * 2 + 1), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(4, i * 2 + 1), &m_Bac_block.TKPsi_a[i]);
        m_VModel->setData(m_VModel->index(4, i * 2 + 6), "TKPsi_b[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(4, i * 2 + 7), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(4, i * 2 + 7), &m_Bac_block.TKPsi_b[i]);
    }

    for (int i = 0; i < 6; i++)
    {
        m_VModel->setData(m_VModel->index(5, i * 2), "DPsi[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(5, i * 2 + 1), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(5, i * 2 + 1), &m_Bac_block.DPsi[i]);
        m_VModel->setData(m_VModel->index(6, i * 2), "TKUa[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(6, i * 2 + 1), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(6, i * 2 + 1), &m_Bac_block.TKUa[i]);
        m_VModel->setData(m_VModel->index(7, i * 2), "TKUb[" + QString::number(i) + "]");
        m_VModel->setData(m_VModel->index(7, i * 2 + 1), CheckDelegate::CheckStyles::OUTVALUE);
        m_VModel->setValueData(m_VModel->index(7, i * 2 + 1), &m_Bac_block.TKUb[i]);
    }
    m_VModel->setData(m_VModel->index(7, 0), "K_freq");
    m_VModel->setData(m_VModel->index(7, 1), CheckDelegate::CheckStyles::OUTVALUE);
    m_VModel->setValueData(m_VModel->index(7, 1), &m_Bac_block.K_freq);
    m_VModel->setData(m_VModel->index(7, 2), "Art");
    m_VModel->setData(m_VModel->index(7, 3), CheckDelegate::CheckStyles::OUTVALUE);
    m_VModel->setValueData(m_VModel->index(7, 3), &m_Bac_block.Art);
    m_VModel->setData(m_VModel->index(7, 4), "Brt");
    m_VModel->setData(m_VModel->index(7, 5), CheckDelegate::CheckStyles::OUTVALUE);
    m_VModel->setValueData(m_VModel->index(7, 5), &m_Bac_block.Brt);
    m_VModel->setData(m_VModel->index(7, 6), "Brt");
    m_VModel->setData(m_VModel->index(7, 7), CheckDelegate::CheckStyles::OUTVALUE);
    m_VModel->setValueData(m_VModel->index(7, 7), &m_Bac_block.Brt);
    m_VModel->setData(m_VModel->index(7, 8), "Tmk0");
    m_VModel->setData(m_VModel->index(7, 9), CheckDelegate::CheckStyles::OUTVALUE);
    m_VModel->setValueData(m_VModel->index(7, 9), &m_Bac_block.Tmk0);
    tv->setModel(m_VModel);
    vlyout->addWidget(tv);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

void TuneKIV::SetDefCoefs()
{
    m_Bac_block.Art = 44.65f;
    m_Bac_block.Brt = 3345.75f;
    m_Bac_block.Tmk0 = 24;
    m_Bac_block.K_freq = 1;

    for (int i = 0; i < 6; i++)
    {
        m_Bac_block.DPsi[i] = 0;
        m_Bac_block.TKUa[i] = 0;
        m_Bac_block.TKUb[i] = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        m_Bac_block.KmU[i] = 1;
        m_Bac_block.KmI1[i] = 1;
        m_Bac_block.KmI16[i] = 1;
        m_Bac_block.KmI2[i] = 1;
        m_Bac_block.KmI32[i] = 1;
        m_Bac_block.KmI8[i] = 1;
        m_Bac_block.N1_TT[i] = 10;
        m_Bac_block.TKPsi_a[i] = 0;
        m_Bac_block.TKPsi_b[i] = 0;
    }
    m_Bac_block.KmI4[0] = 0.997060814f;
    m_Bac_block.KmI4[1] = 0.993458061f;
    m_Bac_block.KmI4[2] = 0.992731500f;

    update();
}
