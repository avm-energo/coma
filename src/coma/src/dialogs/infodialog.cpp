#include "dialogs/infodialog.h"

#include <device/current_device.h>
#include <avm-gen/colors.h>
#include <avm-gen/error.h>
#include <avm-gen/stdfunc.h>
#include <avm-widgets/etabwidget.h>
#include <avm-widgets/lblfunc.h>

#include <QMessageBox>
#include <QVBoxLayout>

InfoDialog::InfoDialog(Device::CurrentDevice *device, QWidget *parent) : UDialog(device, parent)
{
    auto conn = m_device->async();
    auto bsiExt = m_device->bsiExt();
    connect(bsiExt, &Device::BlockStartupInfoExtended::wasUpdated, this, &InfoDialog::syncExt);
    connect(m_device, &Device::CurrentDevice::bsiReceived, this, &InfoDialog::sync);
    connect(this, &InfoDialog::fetchBsi, conn, &AsyncConnection::reqBSI);
    if (m_device->getConfigStorage()->getDeviceSettings().HaveBSIExt())
        connect(this, &InfoDialog::fetchBsi, conn, &AsyncConnection::reqBSIExt);
}

void InfoDialog::setupUI()
{
    const QList<QPair<QString, QString>> dialogPage {
        { m_device->getDeviceName(), "Тип устройства:" },   //
        { "namepo", "Наименование\nпрограммы:" },           //
        { "snle", "Серийный номер\nустройства:" },          //
        { "fwverle", "Версия ПО:" },                        //
        { "verloader", "Верcия Loader:" },                  //
        { "cfcrcle", "КС конфигурации:" },                  //
        { "rstle", "Последний сброс:" },                    //
        { "rstcountle", "Кол-во сбросов:" },                //
        { "cpuidle", "ИД процессора:" },                    //
        { "typeble", "Тип\nбазовой платы:" },               //
        { "snble", "Серийный номер\nбазовой платы:" },      //
        { "hwble", "Аппаратная версия\nбазовой платы:" },   //
        { "typemle", "Тип\nмезонинной платы:" },            //
        { "snmle", "Серийный номер\nмезонинной платы:" },   //
        { "hwmle", "Аппаратная версия\nмезонинной платы:" } //
    };

    auto mainLayout = new QHBoxLayout;
    auto slyout = new QGridLayout;
    for (int i = 0; i < dialogPage.size(); ++i)
    {
        slyout->addWidget(LBLFunc::New(this, dialogPage.at(i).second), i, 0, 1, 1, Qt::AlignRight);
        slyout->addWidget(LBLFunc::New(this, "", dialogPage.at(i).first), i, 1, 1, 1);
    }
    mainLayout->addLayout(slyout, 3);
    mainLayout->addStretch(4);
    setLayout(mainLayout);
}

void InfoDialog::fillBsi()
{
    const auto &bsi = m_device->bsi();
    LBLFunc::setText(this, m_device->getDeviceName(), m_device->getDeviceName());
    LBLFunc::setText(this, "snle", QString::number(bsi.SerialNum, 16));
    LBLFunc::setText(this, "fwverle", StdFunc::VerToStr(bsi.Fwver));
    LBLFunc::setText(this, "cfcrcle", "0x" + QString::number(static_cast<uint>(bsi.Cfcrc), 16));
    LBLFunc::setText(this, "rstle", "0x" + QString::number(bsi.Rst, 16));
    LBLFunc::setText(this, "rstcountle", QString::number(bsi.RstCount));
    LBLFunc::setText(this, "cpuidle",
        QString::number(bsi.UIDHigh, 16) + QString::number(bsi.UIDMid, 16) + QString::number(bsi.UIDLow, 16));
    LBLFunc::setText(this, "typeble", QString::number(bsi.MTypeB, 16));
    LBLFunc::setText(this, "snble", QString::number(bsi.SerialNumB, 16));
    LBLFunc::setText(this, "hwble", StdFunc::VerToStr(bsi.HwverB));
    LBLFunc::setText(this, "typemle", QString::number(bsi.MTypeM, 16));
    LBLFunc::setText(this, "snmle", QString::number(bsi.SerialNumM, 16));
    LBLFunc::setText(this, "hwmle", StdFunc::VerToStr(bsi.HwverM));
}

void InfoDialog::fillBsiExt()
{
    /// TODO: Необходимо вынести все значения из BSI Ext в отдельную вкладку,
    /// в качестве имени использовать адрес значения, выводить всю BSI Ext информацию
    const auto &bsiExt = *m_device->bsiExt();
    LBLFunc::setText(this, "verloader", StdFunc::VerToStr(bsiExt[41]));
    auto namepo = bsiExt[40];
    const char *str = reinterpret_cast<const char *>(&namepo);
    std::string string(str, sizeof(namepo));
    LBLFunc::setText(this, "namepo", QString::fromStdString(string));
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

void InfoDialog::syncExt()
{
    if (updatesEnabled())
        fillBsiExt();
}

void InfoDialog::reqUpdate()
{
    if (updatesEnabled())
        emit fetchBsi();
}
