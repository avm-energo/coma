#include "infodialog.h"

#include "../widgets/wd_func.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <device/current_device.h>
#include <gen/colors.h>
#include <gen/error.h>
#include <gen/stdfunc.h>

InfoDialog::InfoDialog(Device::CurrentDevice *device, QWidget *parent) : UDialog(device, parent)
{
    auto conn = m_device->async();
    auto bsiExt = m_device->bsiExt();
    setupData();
    connect(bsiExt, &Device::BlockStartupInfoExtended::wasUpdated, this, &InfoDialog::syncExt);
    connect(m_device, &Device::CurrentDevice::bsiReceived, this, &InfoDialog::sync);
    connect(this, &InfoDialog::fetchBsi, conn, &AsyncConnection::reqBSI);
    if (m_device->sync()->supportBSIExt())
        connect(this, &InfoDialog::fetchBsi, conn, &AsyncConnection::reqBSIExt);
    else
        QMessageBox::warning(this, "BsiExt", "BsiExt не поддерживается");
}

/// TODO: загрузка настроек по умолчанию должна осуществляться внутри CurrentDevice
/// Настройки должны браться из default.xml
void InfoDialog::setupData()
{
    if (m_device->getConfigStorage()->getDeviceSettings().getBsiExtSettings().empty())
    {
        using namespace Device::XmlDataTypes;
        static const BsiExtItemList defaultBsiExt {
            { 40, BinaryType::string32, true, "Наименование ПО" },                //
            { 41, BinaryType::version32, true, "Версия загрузчика" },             //
            { 42, BinaryType::time32, true, "Время последней поверки модуля" },   //
            { 43, BinaryType::uint32, true, "CRC рег. коэфф. базовой платы" },    //
            { 44, BinaryType::uint32, true, "CRC рег. коэфф. мезонинной платы" }, //
        };
        m_device->bsiExt()->updateStructure(defaultBsiExt);
    }
    else
        m_device->bsiExt()->updateStructure(m_device->getConfigStorage()->getDeviceSettings().getBsiExtSettings());
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
        { "rstcountle", "Количество сбросов:" },            //
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
        slyout->addWidget(WDFunc::NewLBL2(this, dialogPage.at(i).second), i, 0, 1, 1, Qt::AlignRight);
        slyout->addWidget(WDFunc::NewLBL2(this, "", dialogPage.at(i).first), i, 1, 1, 1);
    }
    mainLayout->addLayout(slyout, 3);
    mainLayout->addStretch(4);
    setLayout(mainLayout);
}

void InfoDialog::fillBsi()
{
    const auto &bsi = m_device->bsi();
    WDFunc::SetLBLText(this, m_device->getDeviceName(), m_device->getDeviceName());
    WDFunc::SetLBLText(this, "snle", QString::number(bsi.SerialNum, 16));
    WDFunc::SetLBLText(this, "fwverle", StdFunc::VerToStr(bsi.Fwver));
    WDFunc::SetLBLText(this, "cfcrcle", "0x" + QString::number(static_cast<uint>(bsi.Cfcrc), 16));
    WDFunc::SetLBLText(this, "rstle", "0x" + QString::number(bsi.Rst, 16));
    WDFunc::SetLBLText(this, "rstcountle", QString::number(bsi.RstCount));
    WDFunc::SetLBLText(this, "cpuidle",
        QString::number(bsi.UIDHigh, 16) + QString::number(bsi.UIDMid, 16) + QString::number(bsi.UIDLow, 16));
    WDFunc::SetLBLText(this, "typeble", QString::number(bsi.MTypeB, 16));
    WDFunc::SetLBLText(this, "snble", QString::number(bsi.SerialNumB, 16));
    WDFunc::SetLBLText(this, "hwble", StdFunc::VerToStr(bsi.HwverB));
    WDFunc::SetLBLText(this, "typemle", QString::number(bsi.MTypeM, 16));
    WDFunc::SetLBLText(this, "snmle", QString::number(bsi.SerialNumM, 16));
    WDFunc::SetLBLText(this, "hwmle", StdFunc::VerToStr(bsi.HwverM));
}

void InfoDialog::fillBsiExt()
{
    /// TODO: Необходимо вынести все значения из BSI Ext в отдельную вкладку,
    /// в качестве имени использовать адрес значения, выводить всю BSI Ext информацию
    const auto &bsiExt = *m_device->bsiExt();
    WDFunc::SetLBLText(this, "verloader", StdFunc::VerToStr(bsiExt[41]));
    auto namepo = bsiExt[40];
    const char *str = reinterpret_cast<const char *>(&namepo);
    std::string string(str, sizeof(namepo));
    WDFunc::SetLBLText(this, "namepo", QString::fromStdString(string));
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
