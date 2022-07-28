#include "bd1.h"

#include "../../gen/colors.h"
#include "../../widgets/wd_func.h"

#include <QFrame>
#include <QVBoxLayout>

Bd21_1::Bd21_1(QObject *parent) : DataBlock(parent)
{
    m_blockData = new BlockData;
    setBlock({ 1, "Все", DataTypes::DataBlockTypes::BdBlock, m_blockData, sizeof(BlockData) });
}

void Bd21_1::createWidget()
{
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"
                           "background-color: "
        + QString(Colors::ACONFOCLR) + "; font: bold 10px;}";
    m_widget = new QWidget;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    int i;
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    hlyout->addWidget(WDFunc::NewLBL2(m_widget, "Тек. измерения, приведённые ко входу (ток)"), 0);
    hlyout->addWidget(line, 1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 8; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT2(m_widget, QString::number(i) + ":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdinI" + QString::number(i), ValuesFormat), 1);
        if ((i > 0) && !((i + 1) % 4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    hlyout->addWidget(WDFunc::NewLBL2(m_widget, "Тек. измерения, приведённые ко входу (напряжения)"), 0);
    hlyout->addWidget(line, 1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 8; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT2(m_widget, QString::number(i) + ":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdinU" + QString::number(i), ValuesFormat), 1);
        if ((i > 0) && !((i + 1) % 4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    hlyout->addWidget(WDFunc::NewLBL2(m_widget, "Тек. измерения в инженерных единицах"), 0);
    hlyout->addWidget(line, 1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 8; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT2(m_widget, QString::number(i) + ":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bdain" + QString::number(i), ValuesFormat), 1);
        if ((i > 0) && !((i + 1) % 4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    /*    hlyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Текущие минимумы по каналам");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 8; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(parent, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(parent, "", "bdainn"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Текущие максимумы по каналам");
    hlyout->addWidget(lbl,0);
    hlyout->addWidget(line,1);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    for (i = 0; i < 8; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT(parent, QString::number(i)+":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT(parent, "", "bdainx"+QString::number(i), ValuesFormat), 1);
        if ((i>0)&&!((i+1)%4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    vlyout->addLayout(vlyout); */
    /*    hlyout->addWidget(WDFunc::NewLBLT(parent, "Температура в кристалле:"), 0);
    hlyout->addWidget(WDFunc::NewLBLT(parent, "", "bdat", ValuesFormat), 1);
    vlyout->addLayout(hlyout); */
    vlyout->addStretch(100);
    m_widget->setLayout(vlyout);
}

void Bd21_1::updateWidget()
{
    int i;
    for (i = 0; i < 8; ++i)
    {
        QString tmps = QString::number(i);
        WDFunc::SetLBLText(m_widget, "bdinI" + tmps, QString::number(m_blockData->inI[i], 'f', 5));
        WDFunc::SetLBLText(m_widget, "bdinU" + tmps, QString::number(m_blockData->inU[i], 'f', 5));
        WDFunc::SetLBLText(m_widget, "bdain" + tmps, QString::number(m_blockData->ain[i], 'f', 5));
        //        WDFunc::SetLEData(parent, "bdainn"+tmps, QString::number(m_blockData->ainn[i],'f',5));
        //        WDFunc::SetLEData(parent, "bdainx"+tmps, QString::number(m_blockData->ainx[i],'f',5));
    }
    //    WDFunc::SetLEData(parent, "bdat", QString::number(m_blockData->at,'f',5));
}
