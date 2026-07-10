#include "dialogs/infodialog.h"

#include <libavm-gen/colors.h>
#include <libavm-gen/error.h>
#include <libavm-gen/stdfunc.h>
#include <libavm-widgets/etabwidget.h>
#include <libavm-widgets/lblfunc.h>
#include <libavm-widgets/viewtypewidget.h>
#include <device/configstorage.h>
#include <device/current_device.h>

#include <QMessageBox>
#include <QTabWidget>
#include <QVBoxLayout>

InfoDialog::InfoDialog(Device::CurrentDevice *device, QWidget *parent) : UDialog(device, parent)
{
    auto &bsiExt = m_device->bsiExt();
    connect(&bsiExt, &Device::BlockStartupInfoExtended::wasUpdated, this, &InfoDialog::syncExt);
    connect(m_device, &Device::CurrentDevice::bsiReceived, this, &InfoDialog::sync);
    connect(this, &InfoDialog::fetchBsi, this, &InfoDialog::reqBsi);
    if (m_device->getConfigStorage()->getDeviceSettings().HaveBSIExt())
        connect(this, &InfoDialog::fetchBsi, this, &InfoDialog::reqBsiExt);
}

void InfoDialog::setupUI()
{
    const auto &bsiRecords = m_device->getConfigStorage()->getDeviceSettings().getBsi();
    const auto &bsiExtRecords = m_device->getConfigStorage()->getDeviceSettings().getBsiExt();

    auto tabWidget = new QTabWidget(this);

    // Tab "Основная" — BSI
    auto bsiTab = new QWidget(tabWidget);
    auto bsiLayout = new QVBoxLayout(bsiTab);
    auto bsiGrid = new QGridLayout;
    bsiGrid->setColumnStretch(0, 1);
    bsiGrid->setColumnStretch(1, 1);
    auto devLbl = LBLFunc::New(bsiTab, "Устройство:");
    devLbl->setWordWrap(false);
    bsiGrid->addWidget(devLbl, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    bsiGrid->addWidget(
        ViewType::ViewTypeFunc::New(bsiTab, "devicename", ViewType::ViewTypes::String), 0, 1, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    for (int i = 0; i < int(bsiRecords.size()); ++i)
    {
        const auto &rec = bsiRecords[i];
        auto lbl = LBLFunc::New(bsiTab, rec.desc);
        lbl->setWordWrap(false);
        bsiGrid->addWidget(lbl, i + 1, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
        bsiGrid->addWidget(ViewType::ViewTypeFunc::New(bsiTab, rec.name, rec.type), i + 1, 1, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    }
    bsiLayout->addLayout(bsiGrid, 3);
    bsiLayout->addStretch(400);
    tabWidget->addTab(bsiTab, "Основная");

    // Tab "Расширенная" — BSI Ext
    if (!bsiExtRecords.empty())
    {
        auto bsiExtTab = new QWidget(tabWidget);
        auto bsiExtLayout = new QVBoxLayout(bsiExtTab);
        auto bsiExtGrid = new QGridLayout;
        bsiExtGrid->setColumnStretch(0, 1);
        bsiExtGrid->setColumnStretch(1, 1);
        for (int i = 0; i < int(bsiExtRecords.size()); ++i)
        {
            const auto &rec = bsiExtRecords[i];
            auto lbl = LBLFunc::New(bsiExtTab, rec.desc);
            lbl->setWordWrap(false);
            bsiExtGrid->addWidget(lbl, i, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
            bsiExtGrid->addWidget(ViewType::ViewTypeFunc::New(bsiExtTab, rec.name, rec.type), i, 1, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
        }
        bsiExtLayout->addLayout(bsiExtGrid, 3);
        bsiExtLayout->addStretch(400);
        tabWidget->addTab(bsiExtTab, "Расширенная");
    }

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}

void InfoDialog::fillBsi()
{
    const auto &bsi = m_device->bsi();
    const auto &records = m_device->getConfigStorage()->getDeviceSettings().getBsi();
    ViewType::ViewTypeFunc::setData(this, "devicename", m_device->getDeviceName());
    for (const auto &rec : records)
        ViewType::ViewTypeFunc::setData(this, rec.name, bsi.data(rec.offset));
}

void InfoDialog::fillBsiExt(u32 addr)
{
    u32 realAddr = addr - Device::bsiExtStartReg;
    const Device::BlockStartupInfoExtended &bsiExt = m_device->bsiExt();
    const auto &records = m_device->getConfigStorage()->getDeviceSettings().getBsiExt();
    auto search = std::find_if(records.begin(), records.end(),
        [realAddr](const Device::XmlDataTypes::BsiExtRecord &setting) { return (setting.offset == realAddr); });
    if (search != records.end())
        ViewType::ViewTypeFunc::setData(this, search->name, bsiExt[search->offset]);
}

void InfoDialog::uponInterfaceSetting()
{
    setupUI();
    fillBsi();
}

void InfoDialog::sync()
{
    if (updatesEnabled())
        fillBsi();
}

void InfoDialog::syncExt(u32 addr)
{
    if (updatesEnabled())
        fillBsiExt(addr);
}

void InfoDialog::reqBsi()
{
    m_device->async()->reqBSI();
}

void InfoDialog::reqBsiExt()
{
    auto &bsiExt = m_device->bsiExt();
    m_device->async()->reqBSIExt(bsiExt.endAddr() - bsiExt.startAddr() + 1);
}

void InfoDialog::reqUpdate()
{
    if (updatesEnabled())
        emit fetchBsi();
}
