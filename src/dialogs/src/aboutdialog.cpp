#include "dialogs/aboutdialog.h"

#include <widgets/wd_func.h>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent, Qt::Window | Qt::Dialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void AboutDialog::setupUI()
{
    setWindowTitle("О программе");
    auto hlyout = new QHBoxLayout;
    auto lyout = new QVBoxLayout;
    hlyout->addWidget(WDFunc::NewIcon(this, ":icons/avm-energo.svg"), 0);
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

void AboutDialog::prependLine(const QString &str)
{
    m_lines.prepend(str);
}

void AboutDialog::appendLine(const QString &str)
{
    m_lines.append(str);
}
