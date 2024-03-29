#include "tune82iowidget.h"

#include "../../widgets/wd_func.h"
#include "../mip.h"

#include <QGroupBox>
#include <QScrollArea>

Tune82IoWidget::Tune82IoWidget(Device::CurrentDevice *device, QWidget *parent)
    : QWidget(parent)
    , m_typeM(static_cast<Device::MezzanineBoard>(device->getMezzType()))
    , m_bd1(new Bd182(m_typeM, this))
    , m_mip(new Mip(false, m_typeM, this))
    , m_timer(new QTimer(this))
{
    m_bd1->setup(device->getUID(), device->sync());
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &Tune82IoWidget::updateData);
    setupUI();
}

void Tune82IoWidget::setupUI()
{
    auto mainLayout = new QHBoxLayout;
    auto boxLayout = new QGridLayout;
    auto mipLayout = new QVBoxLayout;
    auto deviceLayout = new QVBoxLayout;
    auto scrollArea = new QScrollArea(this);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    auto box = new QGroupBox("Данные", this);

    mipLayout->addWidget(new QLabel("Данные МИП:", this));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "FreqMIP", "Частота", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "UA_MIP", "UA_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "UB_MIP", "UB_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "UC_MIP", "UC_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "IA_MIP", "IA_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "IB_MIP", "IB_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "IC_MIP", "IC_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiLA_MIP", "PhiLA_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiLB_MIP", "PhiLB_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiLC_MIP", "PhiLC_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiUab_MIP", "PhiUab_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiUbc_MIP", "PhiUbc_MIP", true));
    mipLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiUca_MIP", "PhiUca_MIP", true));

    deviceLayout->addWidget(new QLabel("Данные устройства:", this));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "Freq", "Freq", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "UA", "UA", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "UB", "UB", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "UC", "UC", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "IA", "IA", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "IB", "IB", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "IC", "IC", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiLA", "PhiLA", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiLB", "PhiLB", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiLC", "PhiLC", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiUab", "PhiUab", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiUbc", "PhiUbc", true));
    deviceLayout->addWidget(WDFunc::NewLBLAndLBL(this, "PhiUca", "PhiUca", true));

    auto startBtn = new QPushButton("Запустить обмен данными", this);
    auto cancelBtn = new QPushButton("Выключить обмен данными", this);
    cancelBtn->setEnabled(false);
    connect(startBtn, &QPushButton::clicked, this, //
        [this, startBtn, cancelBtn] {
            m_timer->start();
            startBtn->setEnabled(false);
            cancelBtn->setEnabled(true);
        });
    connect(cancelBtn, &QPushButton::clicked, this, //
        [this, startBtn, cancelBtn] {
            m_timer->stop();
            startBtn->setEnabled(true);
            cancelBtn->setEnabled(false);
        });

    mipLayout->addWidget(startBtn);
    deviceLayout->addWidget(cancelBtn);
    boxLayout->addLayout(mipLayout, 0, 0, 1, 1);
    boxLayout->addLayout(deviceLayout, 0, 1, 1, 1);
    box->setLayout(boxLayout);
    scrollArea->setWidget(box);
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);
}

void Tune82IoWidget::updateData()
{
    float i2nom = 1.0;
    QCoreApplication::processEvents();
    auto mipData = m_mip->takeOneMeasurement(i2nom);
    QCoreApplication::processEvents();
    updateUI(mipData);
    QCoreApplication::processEvents();
    m_bd1->readBlockFromModule();
    QCoreApplication::processEvents();
    auto deviceData = *(m_bd1->data());
    m_calcValues.update(mipData, deviceData);
    QCoreApplication::processEvents();
    updateUI(deviceData);
}

void Tune82IoWidget::updateUI(const MipDataStruct &mipData)
{
    WDFunc::SetLBLText(this, "FreqMIP", QString::number(mipData.freqUPhase[0], 'f', 3));
    WDFunc::SetLBLText(this, "UA_MIP", QString::number(mipData.uPhase[0], 'f', 3));
    WDFunc::SetLBLText(this, "UB_MIP", QString::number(mipData.uPhase[1], 'f', 3));
    WDFunc::SetLBLText(this, "UC_MIP", QString::number(mipData.uPhase[2], 'f', 3));
    WDFunc::SetLBLText(this, "IA_MIP", QString::number(mipData.iPhase[0], 'f', 3));
    WDFunc::SetLBLText(this, "IB_MIP", QString::number(mipData.iPhase[1], 'f', 3));
    WDFunc::SetLBLText(this, "IC_MIP", QString::number(mipData.iPhase[2], 'f', 3));
    WDFunc::SetLBLText(this, "PhiLA_MIP", QString::number(mipData.phiLoadPhase[0], 'f', 3));
    WDFunc::SetLBLText(this, "PhiLB_MIP", QString::number(mipData.phiLoadPhase[1], 'f', 3));
    WDFunc::SetLBLText(this, "PhiLC_MIP", QString::number(mipData.phiLoadPhase[2], 'f', 3));
    WDFunc::SetLBLText(this, "PhiUab_MIP", QString::number(mipData.phiUab, 'f', 3));
    WDFunc::SetLBLText(this, "PhiUbc_MIP", QString::number(mipData.phiUbc, 'f', 3));
    WDFunc::SetLBLText(this, "PhiUca_MIP", QString::number(mipData.phiUca, 'f', 3));
}

void Tune82IoWidget::updateUI(const Bd182::BlockData &devData)
{
    WDFunc::SetLBLText(this, "Freq", QString::number(devData.Frequency, 'f', 3));
    WDFunc::SetLBLText(this, "UA", QString::number(devData.IUefNat_filt[0], 'f', 3));
    WDFunc::SetLBLText(this, "UB", QString::number(devData.IUefNat_filt[1], 'f', 3));
    WDFunc::SetLBLText(this, "UC", QString::number(devData.IUefNat_filt[2], 'f', 3));
    WDFunc::SetLBLText(this, "IA", QString::number(devData.IUefNat_filt[3], 'f', 3));
    WDFunc::SetLBLText(this, "IB", QString::number(devData.IUefNat_filt[4], 'f', 3));
    WDFunc::SetLBLText(this, "IC", QString::number(devData.IUefNat_filt[5], 'f', 3));
    WDFunc::SetLBLText(this, "PhiLA", QString::number(m_calcValues.phiLoad[0], 'f', 3));
    WDFunc::SetLBLText(this, "PhiLB", QString::number(m_calcValues.phiLoad[1], 'f', 3));
    WDFunc::SetLBLText(this, "PhiLC", QString::number(m_calcValues.phiLoad[2], 'f', 3));
    WDFunc::SetLBLText(this, "PhiUab", QString::number(m_calcValues.phiUab, 'f', 3));
    WDFunc::SetLBLText(this, "PhiUbc", QString::number(m_calcValues.phiUbc, 'f', 3));
    WDFunc::SetLBLText(this, "PhiUca", QString::number(m_calcValues.phiUca, 'f', 3));
}
