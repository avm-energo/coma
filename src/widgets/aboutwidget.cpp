#include "aboutwidget.h"

#include "wd_func.h"

#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>

AboutWidget::AboutWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void AboutWidget::setupUI()
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *lyout = new QVBoxLayout;

    hlyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap("images/avm-energo.png")), 0);
    for (const auto &str : qAsConst(m_lines))
    {
        auto *label = WDFunc::NewLBL2(this, str);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        lyout->addWidget(label);
    }
    hlyout->addLayout(lyout, 100);
    lyout = new QVBoxLayout;
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    hlyout->addWidget(WDFunc::NewHexagonPB(
        this, "", [this]() { this->close(); }, ":/icons/tnyes.svg", "Закрыть"));
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}
