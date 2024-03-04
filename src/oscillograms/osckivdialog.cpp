#include "osckivdialog.h"

#include "../widgets/wd_func.h"

#include <device/current_device.h>
#include <limits>

constexpr std::string_view oscFilenumLblFmt { "Текущий номер осциллограммы: %1" };

OscKivDialog::OscKivDialog(Device::CurrentDevice *device, QWidget *parent)
    : UDialog(device, parent)
    , m_commandBtn(nullptr)
    , m_readBtn(nullptr)
    , m_oscFilenumLbl(nullptr)
    , m_reqStateOscTimer(new QTimer(this))
    , m_oscFilenum(0)
    , m_oldOscFilenum(0)
{
    m_reqStateOscTimer->setInterval(100);
    setupUI();
    init();
}

void OscKivDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    m_oscFilenumLbl = WDFunc::NewLBL2(this, oscFilenumLblFmt.data(), "oscFilenumLabel");
    mainLayout->addWidget(m_oscFilenumLbl);
    auto pointsSpinbox = WDFunc::NewSPB2(this, "n_point", 1, 8, 0);
    mainLayout->addWidget(pointsSpinbox);
    auto phaseCombobox = WDFunc::NewCB2(this, "phase", { "Все фазы", "Фаза A", "Фаза B", "Фаза C" });
    phaseCombobox->setCurrentIndex(0);
    mainLayout->addWidget(phaseCombobox);
    auto controlLayout = new QHBoxLayout;
    m_commandBtn = WDFunc::NewPB(this, "commandBtn", "Запуск осциллограмм", this, [this] {
        m_commandBtn->setEnabled(false);
        m_readBtn->setEnabled(false);
        m_device->async()->writeCommand(Commands::C_ReqStateOsc, 1);
    });
    controlLayout->addWidget(m_commandBtn);
    m_readBtn = WDFunc::NewPB(this, "readBtn", "Прочитать осциллограмму", this, [this] {
        m_commandBtn->setEnabled(false);
        m_readBtn->setEnabled(false);
        ;
    });
    m_readBtn->setEnabled(false);
    controlLayout->addWidget(m_readBtn);
    mainLayout->addLayout(controlLayout);
    setLayout(mainLayout);
}

void OscKivDialog::init()
{
    ;
}

void OscKivDialog::updateOscFilenum(const u16 newOscFilenum)
{
    m_oldOscFilenum = m_oscFilenum;
    m_oscFilenum = newOscFilenum;
    if (m_oscFilenum == std::numeric_limits<u16>::min())
        m_oscFilenumLbl->setText("В памяти нет осциллограммы");
    else if (m_oscFilenum == std::numeric_limits<u16>::max())
        m_oscFilenumLbl->setText("Идёт запись осциллограммы");
    else
        m_oscFilenumLbl->setText(QString(oscFilenumLblFmt.data()).arg(m_oscFilenum));
}
