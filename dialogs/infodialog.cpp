#include <QVBoxLayout>
#include "infodialog.h"
#include "../gen/publicclass.h"
#include "../widgets/wd_func.h"

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
    slyout->addWidget(WDFunc::NewLBL(this, ModuleNames()[pc.ModuleBsi.MTypeB]), 0, 0, 1, 1, Qt::AlignRight);
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
    WDFunc::SetLBLText(this, "snle", QString::number(pc.ModuleBsi.SerialNum, 16));
    WDFunc::SetLBLText(this, "fwverle", pc.VerToStr(pc.ModuleBsi.Fwver));
    WDFunc::SetLBLText(this, "cfcrcle", "0x"+QString::number(static_cast<uint>(pc.ModuleBsi.Cfcrc), 16));
    WDFunc::SetLBLText(this, "rstle", "0x"+QString::number(pc.ModuleBsi.Rst, 16));
    WDFunc::SetLBLText(this, "rstcountle", QString::number(pc.ModuleBsi.RstCount, 16));
    WDFunc::SetLBLText(this, "cpuidle", QString::number(pc.ModuleBsi.UIDHigh, 16)+QString::number(pc.ModuleBsi.UIDMid, 16)+QString::number(pc.ModuleBsi.UIDLow, 16));
    WDFunc::SetLBLText(this, "typeble", QString::number(pc.ModuleBsi.MTypeB, 16));
    WDFunc::SetLBLText(this, "snble", QString::number(pc.ModuleBsi.SerialNumB, 16));
    WDFunc::SetLBLText(this, "hwble", pc.VerToStr(pc.ModuleBsi.HwverB));
    WDFunc::SetLBLText(this, "typemle", QString::number(pc.ModuleBsi.MTypeM, 16));
    WDFunc::SetLBLText(this, "snmle", QString::number(pc.ModuleBsi.SerialNumM, 16));
    WDFunc::SetLBLText(this, "hwmle", pc.VerToStr(pc.ModuleBsi.HwverM));
    // расшифровка Hth
    for (int i = 0; i < MAXERRORFLAGNUM; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("hth"+QString::number(i));
        if (lbl == 0)
            return;
        quint32 tmpui = (0x00000001 << i) & pc.ModuleBsi.Hth;
        if (tmpui)
            lbl->setStyleSheet("QLabel {background-color: rgba(255,10,10,255); color: rgba(255,255,255,255);}");
        else
            lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");
    }
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
        if (lbl == 0)
        {
            DBGMSG;
            return;
        }
        lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");
    }
}
