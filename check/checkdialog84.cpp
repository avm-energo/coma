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
#include "../gen/modulebsi.h"
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


CheckDialog84::CheckDialog84(BoardTypes board, QWidget *parent, iec104* channel) : EAbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 6;
    Ch84 = new Check_84(board, parent);
    Ch = new Check;
//    BdNum = 11;
    setAttribute(Qt::WA_DeleteOnClose);
    if(channel != nullptr)
    {
        ch104 = channel;
        connect(ch104,SIGNAL(floatsignalsready(Parse104::FlSignals104*)),this,SLOT(UpdateFlData(Parse104::FlSignals104*)));
        connect(ch104,SIGNAL(sponsignalsready(Parse104::SponSignals104*)),this,SLOT(UpdateSponData(Parse104::SponSignals104*)));
        connect(ch104,SIGNAL(bs104signalsready(Parse104::BS104Signals*)),this,SLOT(UpdateBS104Data(Parse104::BS104Signals*)));
    }

    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(6, &Ch84->Bd_block1, sizeof(Check_84::Bd1));

    if(Config::MTB_A2) //(ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8)
    sl = QStringList() <<  "Основные" << "Дополнительные" << "Отладка";// << "Спорадика";// << "Температура"; //"Общие" <<
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
    /*case 0:
        return Ch->Bd0W(this);*/
    case 0: // Блок #1
        return Ch84->Bd1W(this);
    case 1: // Блок #1
        return Ch84->Bd2W(this);
    case 2: // Блок #1
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
    /*case BD_COMMON:
        Ch->FillBd0(this);
        break;
    case C84_BDA_IN:
        Ch84->FillBd(this);
        break;*/

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

    //if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
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

    //if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    xlsx->write(WRow,29,Ch84->Bd_block1.Tamb,format);

    xlsx->write(WRow,30,Ch84->Bd_block1.Frequency,format);
}
#endif

QWidget *CheckDialog84::CustomTab()
{
    QWidget *w = new QWidget;
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
    w->setLayout(lyout);
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
        Ch84->FillBdUSB(this);
       // Ch84->FillBd2(this);
    }

    if (Commands::GetBd(5, &Ch84->Bd_block5, sizeof(Check_84::Bd5)) == Error::ER_NOERROR)
    {
        Ch84->FillBd5(this);
       // Ch84->FillBd2(this);
    }

    if (Commands::GetBd(8, &Ch84->Bd_block8, sizeof(Check_84::Bd8)) == Error::ER_NOERROR)
    {
        Ch84->FillBd8(this);
       // Ch84->FillBd2(this);
    }
}
void CheckDialog84::UpdateFlData(Parse104::FlSignals104 *Signal)
{
    //if(!first)

    Parse104::FlSignals104 sig = *new Parse104::FlSignals104;
    int i;
    for(i=0; i<Signal->SigNumber; i++)
    {
        sig = *(Signal+i);
        //WDFunc::SetLBLText(Ch, QString::number((Signal+i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal));
        //if((Signal+i)->fl.SigAdr == 101 || (Signal+i)->fl.SigAdr == 102)
        //Ch->FillBd0(this, QString::number((Signal+i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal));

        //if((Signal+i)->fl.SigAdr >= 1000 || (Signal+i)->fl.SigAdr <= 1009)
        Ch84->FillBd(this, QString::number(sig.fl.SigAdr), WDFunc::StringValueWithCheck(sig.fl.SigVal, 3));
    }
}

void CheckDialog84::UpdateSponData(Parse104::SponSignals104* Signal)
{
    Parse104::SponSignals104 sig = *new Parse104::SponSignals104;
    int i,j;
    for(j=0; j<Signal->SigNumber; j++)
    {
      sig = *(Signal+j);

        if((sig.Spon.SigAdr == 3021) || (sig.Spon.SigAdr == 3022) || (sig.Spon.SigAdr == 3023))
        {
            i = sig.Spon.SigAdr-3021;
            if(sig.Spon.SigVal == 1)
            {

               WDFunc::SetLBLTColor(this,QString::number(2423+i), TABCOLORA1); //TABCOLORA1
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(2423+i),ACONFOCLR);
            }

        }

        if(sig.Spon.SigAdr == 3024 || sig.Spon.SigAdr == 3025 || sig.Spon.SigAdr == 3026)
        {
            i = sig.Spon.SigAdr-3024;
            if(sig.Spon.SigVal == 1)
            {
               WDFunc::SetLBLTColor(this,QString::number(2423+i),REDCOLOR);
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(2423+i),ACONFOCLR);
            }

        }

        if(sig.Spon.SigAdr == 3027 || sig.Spon.SigAdr == 3028 || sig.Spon.SigAdr == 3029)
        {
            i = sig.Spon.SigAdr-3027;
            if(sig.Spon.SigVal == 1)
            {
               WDFunc::SetLBLTColor(this,QString::number(2420+i),TABCOLORA1);
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(2420+i),ACONFOCLR);
            }

        }

        if(sig.Spon.SigAdr == 3030 || sig.Spon.SigAdr == 3031 || sig.Spon.SigAdr == 3032)
        {
            i = sig.Spon.SigAdr-3030;
            if(sig.Spon.SigVal == 1)
            {
               WDFunc::SetLBLTColor(this,QString::number(2420+i),REDCOLOR);
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(2420+i),ACONFOCLR);
            }

        }

        if(sig.Spon.SigAdr == 3034)
        {
            if(sig.Spon.SigVal == 1)
            {
               WDFunc::SetLBLTColor(this,QString::number(2432),TABCOLORA1);
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(2432),ACONFOCLR);
            }
        }

        if(sig.Spon.SigAdr == 3035)
        {
            if(sig.Spon.SigVal == 1)
            {
               WDFunc::SetLBLTColor(this,QString::number(2432),REDCOLOR);
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(2432),ACONFOCLR);
            }
        }
    }
}

void CheckDialog84::UpdateBS104Data(Parse104::BS104Signals* Signal)
{
    Parse104::BS104Signals sig = *new Parse104::BS104Signals;
    sig = *Signal;
    ModuleBSI::Bsi* Bsi = new ModuleBSI::Bsi;
    int i;
    int adr = 0;
    memcpy(&adr, &(sig.BS.SigAdr[0]), sizeof(sig.BS.SigAdr));

    for(i=0; i<Signal->SigNumber; i++)
    {
        sig = *(Signal+i);

        Bsi->Hth = sig.BS.SigVal;
        if(adr == 15)
        {
           emit BsiRefresh(Bsi);
        }
    }
}

#endif

void CheckDialog84::UpdateModBusData(ModBusSignal *Signal, int * size)
{

    //ModBusSignal sig = *new ModBusSignal;
    int i = 0;
    for(i=0; i<*size; i++)
    {
      //sig = *(Signal+i);
      Ch84->FillBd(this, QString::number((Signal+i)->SigAdr), WDFunc::StringValueWithCheck((Signal+i)->flVal, 3));
    }
    ModBus::Reading = false;
}

void CheckDialog84::ErrorRead()
{
  //EMessageBox::information(this, "INFO", "Ошибка чтения");
}

void CheckDialog84::onModbusStateChanged(QModbusDevice::State state)
{
    if(state == QModbusDevice::ConnectedState)
    {
     connectionStateRTU =  true;
     EMessageBox::information(this, "Успешно", "Связь по MODBUS установлена");
    }
    else
    {
     connectionStateRTU =  false;
     EMessageBox::information(this, "Провал", "Подключение отсутствует");
    }

}


void CheckDialog84::ConnectMessage(QString* Name)
{
  EMessageBox::information(this, "Успешно", "Связь с "+*Name+" установлена");
}


