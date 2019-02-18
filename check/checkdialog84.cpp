#include <QCoreApplication>
#include <QtMath>
#include <QTime>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include "checkdialog84.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../config/config.h"
#include "../gen/error.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

CheckDialog84::CheckDialog84(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 6;
    Ch84 = new Check_84;
    Ch = new Check;
//    BdNum = 11;

    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(6, &Ch84->Bd_block1, sizeof(Check_84::Bd1));

    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    sl = QStringList() << "Общие" << "Аналоговые" << "Несимметрия" << "Температура";
    else
    sl = QStringList() << "Общие" << "Аналоговые" << "Несимметрия";

    BdUINum = sl.size();

    SetupUI(sl);

    #if PROGSIZE != PROGSIZE_EMUL
    timer->setInterval(ANMEASINT);
   // BdTimer = new QTimer;
   // BdTimer->setInterval(ANMEASINT);
   // connect(BdTimer,SIGNAL(timeout()),this,SLOT(BdTimerTimeout()));
   #endif

}


QWidget *CheckDialog84::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case 0:
        return Ch->Bd0W(this);
    case 1: // Блок #1
        return Ch84->Bd1W(this);
    case 2: // Блок #1
        return Ch84->Bd2W(this);
    case 3: // Блок #1
        return Ch84->Bd3W(this);

    default:
        return new QWidget;
    }
}

#if PROGSIZE != PROGSIZE_EMUL
void CheckDialog84::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        Ch->FillBd0(this);
        break;
    case C84_BDA_IN:
        Ch84->FillBd(this);
        break;

    default:
        return;
    }
}

void CheckDialog84::PrepareHeadersForFile(int row)
{
    QString phase[3] = {"A:","B:","C:"};

    for (int i=0; i<3; i++)
    {
        xlsx->write(row,i+2,QVariant(("Ueff ф")+phase[i]+", кВ"));
        xlsx->write(row,i+5,QVariant("Ieff ф"+phase[i]+", А"));
        xlsx->write(row,i+8,QVariant("Cbush ф"+phase[i]+", пФ"));
        xlsx->write(row,i+11,QVariant("Tg_d ф"+phase[i]+", %"));
        xlsx->write(row,i+14,QVariant("dCbush ф"+phase[i]+", пФ"));
        xlsx->write(row,i+17,QVariant("dTg_d ф"+phase[i]+", %"));
    }
    xlsx->write(row,20,QVariant("U0"));
    xlsx->write(row,21,QVariant("U1"));
    xlsx->write(row,22,QVariant("U2"));
    xlsx->write(row,23,QVariant("I0"));
    xlsx->write(row,24,QVariant("I1"));
    xlsx->write(row,25,QVariant("I2"));
    xlsx->write(row,26,QVariant("Iunb, мА"));
    xlsx->write(row,27,QVariant("Phy_unb, град"));
    xlsx->write(row,28,QVariant("Tmk, °С"));

    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    xlsx->write(row,29,QVariant("Tamb, °С"));

    xlsx->write(row,30,QVariant("Freq, Гц"));
}

void CheckDialog84::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);
    // получение текущих аналоговых сигналов от модуля
    QXlsx::Format format;
    for (int i=0; i<3; i++)
    {
        format.setNumberFormat("0.0000");
        xlsx->write(WRow,i+2, Ch84->Bd_block1.Ueff[i],format);
        xlsx->write(WRow,i+5, Ch84->Bd_block1.Ieff[i],format);
        xlsx->write(WRow,i+8, Ch84->Bd_block1.Cbush[i],format);
        xlsx->write(WRow,i+11,Ch84->Bd_block1.Tg_d[i],format);
        xlsx->write(WRow,i+14,Ch84->Bd_block1.dCbush[i],format);
        xlsx->write(WRow,i+17,Ch84->Bd_block1.dTg_d[i],format);
    }
    xlsx->write(WRow,20,Ch84->Bd_block1.U0,format);
    xlsx->write(WRow,21,Ch84->Bd_block1.U1,format);
    xlsx->write(WRow,22,Ch84->Bd_block1.U2,format);
    xlsx->write(WRow,23,Ch84->Bd_block1.I0,format);
    xlsx->write(WRow,24,Ch84->Bd_block1.I1,format);
    xlsx->write(WRow,25,Ch84->Bd_block1.I2,format);
    xlsx->write(WRow,26,Ch84->Bd_block1.Iunb,format);
    xlsx->write(WRow,27,Ch84->Bd_block1.Phy_unb,format);
    xlsx->write(WRow,28,Ch84->Bd_block1.Tmk,format);

    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    xlsx->write(WRow,29,Ch84->Bd_block1.Tamb,format);

    xlsx->write(WRow,30,Ch84->Bd_block1.Frequency,format);
}
#endif

QWidget *CheckDialog84::CustomTab()
{
    /*QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(Ch84->Bd1W(this));
    QPushButton *pb = new QPushButton("Начать измерения Bd");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StartBdMeasurements()));
#endif
    hlyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения Bd");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StopBdMeasurements()));
#endif
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);*/
    return nullptr;
}

#if PROGSIZE != PROGSIZE_EMUL
void CheckDialog84::ChooseValuesToWrite()
{

}
void CheckDialog84::SetDefaultValuesToWrite()
{

}
void CheckDialog84::PrepareAnalogMeasurements()
{

}

void CheckDialog84::StartBdMeasurements()
{
    BdTimer->start();
}

void CheckDialog84::StopBdMeasurements()
{
    BdTimer->stop();
}

void CheckDialog84::BdTimerTimeout()
{
    if (Commands::GetBd(BdNum, &Ch84->Bd_block1, sizeof(Check_84::Bd1)) == Error::ER_NOERROR)
    {
        Ch84->FillBd(this);
       // Ch84->FillBd2(this);
    }
}
#endif
