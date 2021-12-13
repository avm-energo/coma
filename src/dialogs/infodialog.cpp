#include "infodialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"

#include <QMessageBox>
#include <QVBoxLayout>

InfoDialog::InfoDialog(QWidget *parent) : UDialog(parent)
{

    connect(&Board::GetInstance(), &Board::readyRead, this, &InfoDialog::sync);
    connect(&Board::GetInstance(), &Board::readyReadExt, this, &InfoDialog::syncExt);

    connect(this, &InfoDialog::fetchBsi, BaseInterface::iface(), &BaseInterface::reqBSI);
    if (BaseInterface::iface()->supportBSIExt())
    {
        connect(this, &InfoDialog::fetchBsi, BaseInterface::iface(), &BaseInterface::reqBSIExt);
    }
    else
    {
        QMessageBox::warning(this, "BsiExt", "BsiExt не поддерживается");
    }
}

void InfoDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *slyout = new QGridLayout;

    const QList<QPair<QString, QString>> dialogPage {
        { Board::GetInstance().moduleName(), "Тип устройства:" }, //
        { "namepo", "Наименование программы:" },                  //
        { "snle", "Серийный номер устройства:" },                 //
        { "fwverle", "Версия ПО:" },                              //
        { "verloader", "Верcия Loader:" },                        //
        { "cfcrcle", "КС конфигурации:" },                        //
        { "rstle", "Последний сброс:" },                          //
        { "rstcountle", "Количество сбросов:" },                  //
        { "cpuidle", "ИД процессора:" },                          //
        { "typeble", "Тип базовой платы:" },                      //
        { "snble", "Серийный номер базовой платы:" },             //
        { "hwble", "Аппаратная версия базовой платы:" },          //
        { "typemle", "Тип мезонинной платы:" },                   //
        { "snmle", "Серийный номер мезонинной платы:" },          //
        { "hwmle", "Аппаратная версия мезонинной платы:" }        //

    };
    for (int i = 0; i < dialogPage.size(); ++i)
    {
        slyout->addWidget(WDFunc::NewLBL2(this, dialogPage.at(i).second), i, 0, 1, 1, Qt::AlignRight);
        slyout->addWidget(WDFunc::NewLBL2(this, "", dialogPage.at(i).first), i, 1, 1, 1);
    }

    slyout->setColumnStretch(1, 1);
    lyout->addLayout(slyout);
    lyout->addStretch(1);
    setLayout(lyout);
}

void InfoDialog::FillBsi()
{

    const auto bsi = Board::GetInstance().baseSerialInfo();

    WDFunc::SetLBLText(this, Board::GetInstance().moduleName(), Board::GetInstance().moduleName());
    WDFunc::SetLBLText(this, "snle", QString::number(bsi.SerialNum, 16));
    WDFunc::SetLBLText(this, "fwverle", StdFunc::VerToStr(bsi.Fwver));
    WDFunc::SetLBLText(this, "cfcrcle", "0x" + QString::number(static_cast<uint>(bsi.Cfcrc), 16));
    WDFunc::SetLBLText(this, "rstle", "0x" + QString::number(bsi.Rst, 16));
    WDFunc::SetLBLText(this, "rstcountle", QString::number(bsi.RstCount, 16));
    WDFunc::SetLBLText(this, "cpuidle",
        QString::number(bsi.UIDHigh, 16) + QString::number(bsi.UIDMid, 16) + QString::number(bsi.UIDLow, 16));
    WDFunc::SetLBLText(this, "typeble", QString::number(bsi.MTypeB, 16));
    WDFunc::SetLBLText(this, "snble", QString::number(bsi.SerialNumB, 16));
    WDFunc::SetLBLText(this, "hwble", StdFunc::VerToStr(bsi.HwverB));
    WDFunc::SetLBLText(this, "typemle", QString::number(bsi.MTypeM, 16));
    WDFunc::SetLBLText(this, "snmle", QString::number(bsi.SerialNumM, 16));
    WDFunc::SetLBLText(this, "hwmle", StdFunc::VerToStr(bsi.HwverM));
}

void InfoDialog::FillBsiExt()
{
    const auto bsi = Board::GetInstance().baseSerialInfoExt();
    WDFunc::SetLBLText(this, "verloader", StdFunc::VerToStr(bsi.VerLoader));
    const char *str = reinterpret_cast<const char *>(&bsi.NamePO);
    std::string string(str, sizeof(quint32));
    WDFunc::SetLBLText(this, "namepo", QString::fromStdString(string));
}

void InfoDialog::uponInterfaceSetting()
{
    SetupUI();
    FillBsi();
}

void InfoDialog::sync()
{
    if (updatesEnabled())
    {
        FillBsi();
    }
}

void InfoDialog::syncExt()
{
    if (updatesEnabled())
    {
        FillBsiExt();
    }
}

void InfoDialog::reqUpdate()
{
    if (updatesEnabled())
    {
        emit fetchBsi();
    }
}
