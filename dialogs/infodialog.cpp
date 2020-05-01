#include <QVBoxLayout>
#include "infodialog.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"
#include "../gen/mainwindow.h"

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent)
{
    SetupUI();
}

void InfoDialog::SetupUI()
{
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *slyout = new QGridLayout;
    slyout->addWidget(WDFunc::NewLBL(this, "Тип устройства:"), 0, 0, 1, 1, Qt::AlignRight);
    slyout->addWidget(WDFunc::NewLBL(this, "АВМ-КИВ"), 0, 1, 1, 1); //ModuleBSI::GetModuleTypeString())
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
    WDFunc::SetLBLText(this, "cfcrcle", "0x"+QString::number(static_cast<uint>(bsi.Cfcrc), 16));
    WDFunc::SetLBLText(this, "rstle", "0x"+QString::number(bsi.Rst, 16));
    WDFunc::SetLBLText(this, "rstcountle", QString::number(bsi.RstCount, 16));
    WDFunc::SetLBLText(this, "cpuidle", QString::number(bsi.UIDHigh, 16)+QString::number(bsi.UIDMid, 16)+QString::number(bsi.UIDLow, 16));
    WDFunc::SetLBLText(this, "typeble", QString::number(bsi.MTypeB, 16));
    WDFunc::SetLBLText(this, "snble", QString::number(bsi.SerialNumB, 16));
    WDFunc::SetLBLText(this, "hwble", StdFunc::VerToStr(bsi.HwverB));
    WDFunc::SetLBLText(this, "typemle", QString::number(bsi.MTypeM, 16));
    WDFunc::SetLBLText(this, "snmle", QString::number(bsi.SerialNumM, 16));
    WDFunc::SetLBLText(this, "hwmle", StdFunc::VerToStr(bsi.HwverM));
    // расшифровка Hth
}

void InfoDialog::FillBsiFrom104(Parse104::BS104Signals* BS104)
{
    //Parse104::BS104Signals sig = *new Parse104::BS104Signals;
    //sig = *BS104;
    ModuleBSI::Bsi bsi = *new ModuleBSI::Bsi;
    int i;
    int startadr = 0;
    memcpy(&startadr, &(BS104->BS.SigAdr[0]), sizeof(BS104->BS.SigAdr));

    if(BS104->SigNumber && (startadr >= 1 && startadr <= 15))
    {
        for(i=0; i<BS104->SigNumber; i++)
        memcpy((((quint32*)(&ModuleBSI::ModuleBsi)+(i+startadr-1))), (((quint32*)(&BS104->BS.SigVal)+4*i)), sizeof(BS104->BS.SigVal));

        MainWindow::MTypeB = ModuleBSI::ModuleBsi.MTypeB;
        MainWindow::MTypeM = ModuleBSI::ModuleBsi.MTypeM;
        //ModuleBSI::ModuleBsi.Hth = bsi.Hth;

        WDFunc::SetLBLText(this, "snle", QString::number(ModuleBSI::ModuleBsi.SerialNum, 16));
        WDFunc::SetLBLText(this, "fwverle", StdFunc::VerToStr(ModuleBSI::ModuleBsi.Fwver));
        WDFunc::SetLBLText(this, "cfcrcle", "0x"+QString::number(static_cast<uint>(ModuleBSI::ModuleBsi.Cfcrc), 16));
        WDFunc::SetLBLText(this, "rstle", "0x"+QString::number(ModuleBSI::ModuleBsi.Rst, 16));
        WDFunc::SetLBLText(this, "rstcountle", QString::number(ModuleBSI::ModuleBsi.RstCount, 16));
        WDFunc::SetLBLText(this, "cpuidle", QString::number(ModuleBSI::ModuleBsi.UIDHigh, 16)+QString::number(ModuleBSI::ModuleBsi.UIDMid, 16)+QString::number(ModuleBSI::ModuleBsi.UIDLow, 16));
        WDFunc::SetLBLText(this, "typeble", QString::number(ModuleBSI::ModuleBsi.MTypeB, 16));
        WDFunc::SetLBLText(this, "snble", QString::number(ModuleBSI::ModuleBsi.SerialNumB, 16));
        WDFunc::SetLBLText(this, "hwble", StdFunc::VerToStr(ModuleBSI::ModuleBsi.HwverB));
        WDFunc::SetLBLText(this, "typemle", QString::number(ModuleBSI::ModuleBsi.MTypeM, 16));
        WDFunc::SetLBLText(this, "snmle", QString::number(ModuleBSI::ModuleBsi.SerialNumM, 16));
        WDFunc::SetLBLText(this, "hwmle", StdFunc::VerToStr(ModuleBSI::ModuleBsi.HwverM));

        // расшифровка Hth
    }

}

void InfoDialog::FillBsiFromModBus(ModBusBSISignal *Signal, int * size)
{
    //Parse104::BS104Signals sig = *new Parse104::BS104Signals;
    //sig = *BS104;
    ModuleBSI::Bsi bsi = *new ModuleBSI::Bsi;
    int i;
    int startadr = 0;
    memcpy(&startadr, &(Signal->SigAdr), sizeof(Signal->SigAdr));

    if(*size && startadr == 1)
    {
        for(i=0; i< *size; i++)
        memcpy((((quint32*)(&bsi)+i)), ((quint32*)(&Signal->Val)+2*i), sizeof(Signal->Val));

        MainWindow::MTypeB = bsi.MTypeB;
        MainWindow::MTypeM = bsi.MTypeM;
        ModuleBSI::ModuleBsi.Hth = bsi.Hth;

        WDFunc::SetLBLText(this, "snle", QString::number(bsi.SerialNum, 16));
        WDFunc::SetLBLText(this, "fwverle", StdFunc::VerToStr(bsi.Fwver));
        WDFunc::SetLBLText(this, "cfcrcle", "0x"+QString::number(static_cast<uint>(bsi.Cfcrc), 16));
        WDFunc::SetLBLText(this, "rstle", "0x"+QString::number(bsi.Rst, 16));
        WDFunc::SetLBLText(this, "rstcountle", QString::number(bsi.RstCount, 16));
        WDFunc::SetLBLText(this, "cpuidle", QString::number(bsi.UIDHigh, 16)+QString::number(bsi.UIDMid, 16)+QString::number(bsi.UIDLow, 16));
        WDFunc::SetLBLText(this, "typeble", QString::number(bsi.MTypeB, 16));
        WDFunc::SetLBLText(this, "snble", QString::number(bsi.SerialNumB, 16));
        WDFunc::SetLBLText(this, "hwble", StdFunc::VerToStr(bsi.HwverB));
        WDFunc::SetLBLText(this, "typemle", QString::number(bsi.MTypeM, 16));
        WDFunc::SetLBLText(this, "snmle", QString::number(bsi.SerialNumM, 16));
        WDFunc::SetLBLText(this, "hwmle", StdFunc::VerToStr(bsi.HwverM));
        // расшифровка Hth
    }


    ModBus::Reading = false;

}

void InfoDialog::ClearBsi()
{
    WDFunc::SetLBLText(this, "snle", "");
    WDFunc::SetLBLText(this, "fwverle", "");
    WDFunc::SetLBLText(this, "cfcrcle", "");
    WDFunc::SetLBLText(this, "rstle", "");
    WDFunc::SetLBLText(this, "rstcountle", "");
    WDFunc::SetLBLText(this, "cpuidle", "");
    WDFunc::SetLBLText(this, "typeble", "");
    WDFunc::SetLBLText(this, "snble", "");
    WDFunc::SetLBLText(this, "hwble", "");
    WDFunc::SetLBLText(this, "typemle", "");
    WDFunc::SetLBLText(this, "snmle", "");
    WDFunc::SetLBLText(this, "hwmle", "");
    // расшифровка Hth
    for (int i = 0; i < 32; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("hth"+QString::number(i));
        if (lbl == nullptr)
        {
            DBGMSG;
            return;
        }
        lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");
    }
}
