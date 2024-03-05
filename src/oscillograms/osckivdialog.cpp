#include "osckivdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <device/current_device.h>
#include <gen/stdfunc.h>
#include <limits>

constexpr std::string_view oscFilenumLblFmt { "Текущий номер осциллограммы: %1" };
constexpr u32 oscStateAddr = 13100;

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
    QObject::connect(m_reqStateOscTimer, &QTimer::timeout, this, &OscKivDialog::reqOscState);
    setupUI();
    reqOscState();
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
    m_commandBtn = WDFunc::NewPB(this, "commandBtn", "Запуск осциллограмм", this, &OscKivDialog::writeTypeOsc);
    controlLayout->addWidget(m_commandBtn);
    m_readBtn = WDFunc::NewPB(this, "readBtn", "Прочитать осциллограмму", this, &OscKivDialog::reqOscFile);
    m_readBtn->setEnabled(false);
    controlLayout->addWidget(m_readBtn);
    mainLayout->addLayout(controlLayout);
    setLayout(mainLayout);
}

void OscKivDialog::updateOscFilenum(const u32 newOscFilenum)
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

void OscKivDialog::enableButtons(const bool enabling)
{
    m_commandBtn->setEnabled(enabling);
    m_readBtn->setEnabled(enabling);
}

void OscKivDialog::reqOscState()
{
    m_device->async()->reqBitStrings(oscStateAddr, 1);
}

void OscKivDialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (bs.sigAdr == oscStateAddr)
    {
        updateOscFilenum(bs.sigVal);
    }
}

void OscKivDialog::writeTypeOsc()
{
    // enableButtons(false);
    TypeOsc command { 0, 0, 0 };
    command.n_point = WDFunc::SPBData<u8>(this, "n_point");
    command.phase = WDFunc::CBIndex(this, "phase");
    DataTypes::BlockStruct block { 13, StdFunc::toByteArray(command) };
    m_device->async()->writeCommand(Commands::C_WriteTypeOsc, QVariant::fromValue(block));
}

void OscKivDialog::reqOscFile()
{
    // enableButtons(true);
}
