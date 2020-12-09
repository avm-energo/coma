#include "infodialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"
#ifdef _DEBUG
#include <QDebug>
#include <QElapsedTimer>
#endif
#include <QVBoxLayout>

InfoDialog::InfoDialog(QWidget *parent) : UDialog(parent)
{
    m_oneShotUpdateFlag = false;
    connect(&Board::GetInstance(), &Board::readyRead, this, &InfoDialog::sync);
}

void InfoDialog::SetupUI()
{
    QString tmps = "QDialog {background-color: " + QString(Colors::MAINWINCLRA1) + ";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *slyout = new QGridLayout;

    const QList<QPair<QString, QString>> dialogPage {
        { Board::GetInstance().moduleName(), "Тип устройства:" }, //
        { "snle", "Серийный номер устройства:" },                 //
        { "fwverle", "Версия ПО:" },                              //
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
        slyout->addWidget(WDFunc::NewLBL(this, dialogPage.at(i).second), i, 0, 1, 1, Qt::AlignRight);
        slyout->addWidget(WDFunc::NewLBLT(this, "", dialogPage.at(i).first), i, 1, 1, 1);
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

void InfoDialog::uponInterfaceSetting()
{
    SetupUI();
    FillBsi();
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

void InfoDialog::sync()
{
    if (m_updatesEnabled)
    {
        if (!m_oneShotUpdateFlag)
        {
            m_oneShotUpdateFlag = true;
            FillBsi();
        }
    }
    else
        m_oneShotUpdateFlag = false;
}

void InfoDialog::reqUpdate()
{
    if (m_updatesEnabled)
        BaseInterface::iface()->reqBSI();
}
