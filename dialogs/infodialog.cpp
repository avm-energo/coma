#include "infodialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"

#include <QVBoxLayout>

InfoDialog::InfoDialog(QWidget *parent) : UDialog(parent)
{
    SetupUI();
    FillBsi();
}

void InfoDialog::SetupUI()
{
    Inf[MTYPE_KIV] = "АВМ-КИВ";
    Inf[MTYPE_KTF] = "АВМ-КТФ";
    Inf[MTYPE_KDV] = "АВМ-КДВ";

    QString tmps = "QDialog {background-color: " + QString(Colors::MAINWINCLRA1) + ";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *slyout = new QGridLayout;
    slyout->addWidget(WDFunc::NewLBL(this, "Тип устройства:"), 0, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBL(this, Inf[Board::GetInstance().type()]), 0, 1, 1, 1);
    // ModuleBSI::GetModuleTypeString())
    slyout->addWidget(WDFunc::NewLBL(this, "Серийный номер устройства:"), 1, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "snle"), 1, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Версия ПО:"), 2, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "fwverle"), 2, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "КС конфигурации:"), 3, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "cfcrcle"), 3, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Последний сброс:"), 4, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "rstle"), 4, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Количество сбросов:"), 5, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "rstcountle"), 5, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "ИД процессора:"), 6, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "cpuidle"), 6, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Тип базовой платы:"), 7, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "typeble"), 7, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Серийный номер базовой платы:"), 8, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "snble"), 8, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Аппаратная версия базовой платы:"), 9, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "hwble"), 9, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Тип мезонинной платы:"), 10, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "typemle"), 10, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Серийный номер мезонинной платы:"), 11, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "snmle"), 11, 1, 1, 1);
    slyout->addWidget(WDFunc::NewLBL(this, "Аппаратная версия мезонинной платы:"), 12, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBLT(this, "", "hwmle"), 12, 1, 1, 1);
    slyout->setColumnStretch(1, 1);
    lyout->addLayout(slyout);
    lyout->addStretch(1);
    setLayout(lyout);
}

void InfoDialog::FillBsi()
{
    ModuleBSI::Bsi bsi = ModuleBSI::GetBsi();
    ModuleBSI::ModuleBsi.Hth = bsi.Hth;
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

void InfoDialog::ClearBsi()
{
    QList<QLabel *> allLabels = this->findChildren<QLabel *>();
    for (QLabel *label : allLabels)
    {
        label->clear();
    }
    //    WDFunc::SetLBLText(this, "snle", "");
    //    WDFunc::SetLBLText(this, "fwverle", "");
    //    WDFunc::SetLBLText(this, "cfcrcle", "");
    //    WDFunc::SetLBLText(this, "rstle", "");
    //    WDFunc::SetLBLText(this, "rstcountle", "");
    //    WDFunc::SetLBLText(this, "cpuidle", "");
    //    WDFunc::SetLBLText(this, "typeble", "");
    //    WDFunc::SetLBLText(this, "snble", "");
    //    WDFunc::SetLBLText(this, "hwble", "");
    //    WDFunc::SetLBLText(this, "typemle", "");
    //    WDFunc::SetLBLText(this, "snmle", "");
    //    WDFunc::SetLBLText(this, "hwmle", "");
    // расшифровка Hth
}

void InfoDialog::reqUpdate()
{
    QList<DataTypes::SignalsStruct> list;

    if (DataManager::getSignals(BSIREG, BSIENDREG, DataTypes::BitString, list) != Error::Msg::ResEmpty) { }
}
