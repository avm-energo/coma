#include "aboutwidget.h"

#include "wd_func.h"

AboutWidget::AboutWidget(QWidget *parent) : QDialog(parent, Qt::Window | Qt::Dialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void AboutWidget::setupUI()
{
    auto hlyout = new QHBoxLayout;
    auto lyout = new QVBoxLayout;

    hlyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap("images/avm-energo.png")), 0);
    for (const auto &str : qAsConst(m_lines))
    {
        auto label = WDFunc::NewLBL2(this, str);
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
    setFixedSize(QSize(450, 400));
}

void AboutWidget::prependLine(const QString &str)
{
    m_lines.prepend(str);
}

void AboutWidget::appendLine(const QString &str)
{
    m_lines.append(str);
}
