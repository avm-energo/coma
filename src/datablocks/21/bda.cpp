#include "bda.h"

#include "../../gen/colors.h"
#include "../../widgets/wd_func.h"

#include <QFrame>
#include <QVBoxLayout>

Bda::Bda(QObject *parent) : DataBlock(parent)
{
    m_blockData = new BlockData;
    setBlock({ 1, "Сырые измерения", DataTypes::DataBlockTypes::BdaBlock, m_blockData, sizeof(BlockData) });
}

void Bda::createWidget()
{
    m_widget = new QWidget;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    for (int i = 0; i < 8; ++i)
    {
        hlyout->addWidget(WDFunc::NewLBLT2(m_widget, QString::number(i) + ":"), 0);
        hlyout->addWidget(WDFunc::NewLBLT2(m_widget, "", "bda" + QString::number(i)), 1);
        if ((i > 1) && !((i + 1) % 4))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    m_widget->setLayout(vlyout);
}

void Bda::updateWidget()
{
    int i;
    for (i = 0; i < 8; ++i)
        WDFunc::SetLBLText(m_widget, "bda" + QString::number(i), QString::number(m_blockData->sin[i]));
}
