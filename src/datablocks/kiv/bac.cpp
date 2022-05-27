#include "bac.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>

Bac::Bac(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 2, "Настроечные параметры", DataTypes::DataBlockTypes::BacBlock, m_blockData.get(), sizeof(BlockData) });
    createWidget();
    readBlockFromModule();
    updateWidget();
}

void Bac::createWidget()
{
    //    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
    //                           "background-color: "
    //        + QString(Colors::ColorsMap[Colors::AConfO]) + "; font: bold 10px;}";
    //    QString ValuesLEFormat = QString(Colors::ColorsMap[Colors::AConfO]);
    m_widget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    //    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox(block().caption);
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
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "N1_TT[" + QString::number(i) + "]"), 0, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i), "", "Число витков первичной обмотки"), 1, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "KmU[" + QString::number(i) + "]"), 0, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 3), "", "Коэффициенты по напряжению"),
            1, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "KmI1[" + QString::number(i) + "]"), 2, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 6), "", "Коэффициенты по току для Кацп=1"), 3, i, 1,
            1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "KmI2[" + QString::number(i) + "]"), 2, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 9), "", "Коэффициенты по току для Кацп=2"), 3, i + 3,
            1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "KmI4[" + QString::number(i) + "]"), 4, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 12), "", "Коэффициенты по току для Кацп=4"), 5, i, 1,
            1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "KmI8[" + QString::number(i) + "]"), 4, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 15), "", "Коэффициенты по току для Кацп=8"), 5, i + 3,
            1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "KmI16[" + QString::number(i) + "]"), 6, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 18), "", "Коэффициенты по току для Кацп=16"), 7, i, 1,
            1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "KmI32[" + QString::number(i) + "]"), 6, i + 3, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 21), "", "Коэффициенты по току для Кацп=32"), 7,
            i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "TKPsi_a[" + QString::number(i) + "]"), 8, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 24), "",
                              "Коэффициенты линейной коррекции по tg delta"),
            9, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "TKPsi_b[" + QString::number(i) + "]"), 8, i + 3, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 27), "",
                              "Коэффициенты квадратичной коррекции по tg delta"),
            9, i + 3, 1, 1);
    }

    for (int i = 0; i < 6; i++)
    {
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "DPsi[" + QString::number(i) + "]"), 10, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 30), "", "Коррекция фазы в градусах"), 11, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "TKUa[" + QString::number(i) + "]"), 12, i, 1, 1);
        glyout->addWidget(
            WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 36), "", "Коэффициенты линейной коррекции по U и I"),
            13, i, 1, 1);
        glyout->addWidget(WDFunc::NewLBL2(m_widget, "TKUb[" + QString::number(i) + "]"), 14, i, 1, 1);
        glyout->addWidget(WDFunc::NewLE2(m_widget, "tune" + QString::number(i + 42), "",
                              "Коэффициенты квадратичной коррекции по U и I"),
            15, i, 1, 1);
    }

    glyout->addWidget(WDFunc::NewLBL2(m_widget, "K_freq"), 16, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE2(m_widget, "tune48", "", "Коррекция частоты"), 17, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(m_widget, "Art"), 16, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLE2(m_widget, "tune49", "", "Коэффициент в канале Pt100"), 17, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(m_widget, "Brt"), 16, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLE2(m_widget, "tune50", "", "Смещение в канале Pt100"), 17, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(m_widget, "Tmk0"), 16, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLE2(m_widget, "tune51", "", "Начальная температура МК для коррекции"), 17, 3, 1, 1);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    lyout->addWidget(blockButtonsUI());
    lyout->addStretch(100);
    m_widget->setLayout(lyout);
    m_widgetIsSet = true;
    //    return m_widget;
}

void Bac::setDefBlock()
{
    m_blockData->Art = 81.5f;
    m_blockData->Brt = 6033.0f;
    m_blockData->Tmk0 = 24;
    m_blockData->K_freq = 1;

    for (int i = 0; i < 6; i++)
    {
        m_blockData->DPsi[i] = 0;
        m_blockData->TKUa[i] = 0;
        m_blockData->TKUb[i] = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        m_blockData->KmU[i] = 1;
        m_blockData->KmI1[i] = 1;
        m_blockData->KmI16[i] = 1;
        m_blockData->KmI2[i] = 1;
        m_blockData->KmI32[i] = 1;
        m_blockData->KmI4[i] = 1;
        m_blockData->KmI8[i] = 1;
        m_blockData->N1_TT[i] = 10;
        m_blockData->TKPsi_a[i] = 0;
        m_blockData->TKPsi_b[i] = 0;
    }
    //    m_blockData->KmI4[0] = 0.997060814f;
    //    m_blockData->KmI4[1] = 0.993458061f;
    //    m_blockData->KmI4[2] = 0.992731500f;
}

void Bac::updateWidget()
{
    if (m_widgetIsSet)
    {
        for (int i = 0; i < 3; i++)
        {
            WDFunc::SetLEData(m_widget, "tune" + QString::number(i), QString::number(m_blockData->N1_TT[i], 'g', 5));
            WDFunc::SetLEData(m_widget, "tune" + QString::number(i + 3), QString::number(m_blockData->KmU[i], 'f', 5));
            WDFunc::SetLEData(m_widget, "tune" + QString::number(i + 6), QString::number(m_blockData->KmI1[i], 'f', 5));
            WDFunc::SetLEData(m_widget, "tune" + QString::number(i + 9), QString::number(m_blockData->KmI2[i], 'f', 5));
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 12), QString::number(m_blockData->KmI4[i], 'f', 5));
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 15), QString::number(m_blockData->KmI8[i], 'f', 5));
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 18), QString::number(m_blockData->KmI16[i], 'f', 5));
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 21), QString::number(m_blockData->KmI32[i], 'f', 5));
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 24), QString::number(m_blockData->TKPsi_a[i], 'e', 5));
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 27), QString::number(m_blockData->TKPsi_b[i], 'e', 5));
        }

        for (int i = 0; i < 6; i++)
        {
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 30), QString::number(m_blockData->DPsi[i], 'f', 5));
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 36), QString::number(m_blockData->TKUa[i], 'e', 5));
            WDFunc::SetLEData(
                m_widget, "tune" + QString::number(i + 42), QString::number(m_blockData->TKUb[i], 'e', 5));
        }

        WDFunc::SetLEData(m_widget, "tune48", QString::number(m_blockData->K_freq, 'f', 5));
        WDFunc::SetLEData(m_widget, "tune49", QString::number(m_blockData->Art, 'f', 5));
        WDFunc::SetLEData(m_widget, "tune50", QString::number(m_blockData->Brt, 'f', 5));
        WDFunc::SetLEData(m_widget, "tune51", QString::number(m_blockData->Tmk0, 'f', 5));
    }
}

void Bac::updateFromWidget()
{
    if (m_widgetIsSet)
    {
        for (int i = 0; i < 3; i++)
        {
            m_blockData->N1_TT[i] = WDFunc::LEData(m_widget, "tune" + QString::number(i)).toUInt();
            m_blockData->KmU[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 3)));
            m_blockData->KmI1[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 6)));
            m_blockData->KmI2[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 9)));
            m_blockData->KmI4[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 12)));
            m_blockData->KmI8[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 15)));
            m_blockData->KmI16[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 18)));
            m_blockData->KmI32[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 21)));
            m_blockData->TKPsi_a[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 24)));
            m_blockData->TKPsi_b[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 27)));
        }

        for (int i = 0; i < 6; i++)
        {
            m_blockData->DPsi[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 30)));
            m_blockData->TKUa[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 36)));
            m_blockData->TKUb[i] = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune" + QString::number(i + 42)));
        }
        m_blockData->K_freq = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune48"));
        m_blockData->Art = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune49"));
        m_blockData->Brt = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune50"));
        m_blockData->Tmk0 = StdFunc::toFloat(WDFunc::LEData(m_widget, "tune51"));
    }
}

Bac::BlockData *Bac::data()
{
    return m_blockData.get();
}
