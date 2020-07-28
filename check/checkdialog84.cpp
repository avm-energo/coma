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
#include "../gen/colors.h"
#include "../config/config.h"
#include "../gen/error.h"
#include "../usb/commands.h"


CheckDialog84::CheckDialog84(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QStringList sl;
    BdNum = 6;
    Ch84 = new Check_84;
    Ch = new Check;
//    BdNum = 11;
    setAttribute(Qt::WA_DeleteOnClose);

    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(6, &Ch84->Bd_block1, sizeof(Check_84::Bd1));

    if(Config::MTB_A2) //(ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8)
    sl = QStringList() <<  "Основные" << "Дополнительные" << "Отладка";// << "Спорадика";// << "Температура"; //"Общие" <<
    else
    sl = QStringList() << "Общие" << "Аналоговые" << "Несимметрия";

    BdUINum = sl.size();

    SetupUI(sl);

    Timer->setInterval(ANMEASINT);
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

void CheckDialog84::RefreshAnalogValues(int bdnum)
{
    Q_UNUSED(bdnum)
/*    switch (bdnum)
    {
    case BD_COMMON:
        Ch->FillBd0(this);
        break;
    case C84_BDA_IN:
        Ch84->FillBd(this);
        break;

    default:
        return;
    } */
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

QWidget *CheckDialog84::CustomTab()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(Ch84->Bd1W(this));
    QPushButton *pb = new QPushButton("Начать измерения Bd");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StartBdMeasurements()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения Bd");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StopBdMeasurements()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return nullptr;
}

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

void CheckDialog84::USBUpdate()
{
    if (Commands::GetBd(BdNum, &Ch84->Bd_block1, sizeof(Check_84::Bd1)) == NOERROR)
    {
        Ch84->FillBdUSB(this);
       // Ch84->FillBd2(this);
    }

    if (Commands::GetBd(5, &Ch84->Bd_block5, sizeof(Check_84::Bd5)) == NOERROR)
    {
        Ch84->FillBd5(this);
       // Ch84->FillBd2(this);
    }

    if (Commands::GetBd(8, &Ch84->Bd_block8, sizeof(Check_84::Bd8)) == NOERROR)
    {
        Ch84->FillBd8(this);
       // Ch84->FillBd2(this);
    }
}
void CheckDialog84::UpdateFlData(IEC104Thread::FlSignals104 *Signal)
{
    IEC104Thread::FlSignals104 sig = *new IEC104Thread::FlSignals104;
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

void CheckDialog84::UpdateSponData(IEC104Thread::SponSignals *Signal)
{
    int i, j;
    for(j=0; j<Signal->SigNumber; j++)
    {
        quint32 sigadr = Signal->Spon[j].SigAdr;
        quint8 sigval = Signal->Spon[j].SigVal;
        if ((sigadr >= 3011) && (sigadr <= 3013))
        {
            i = sigadr-3011;
            WDFunc::SetLBLTColor(this,QString::number(1000+i), (sigval == 1) ? TABCOLORA1 : ACONFOCLR); //TABCOLORA1
        }
        if ((sigadr >= 3014) && (sigadr <= 3016))
        {
            i = sigadr-3014;
            WDFunc::SetLBLTColor(this,QString::number(1100+i), (sigval == 1) ? TABCOLORA1 : ACONFOCLR); //TABCOLORA1
        }
        if ((sigadr >= 3018) && (sigadr <= 3020))
        {
            i = sigadr-3018;
            WDFunc::SetLBLTColor(this,QString::number(1000+i), (sigval == 1) ? TABCOLORA1 : ACONFOCLR); //TABCOLORA1
        }
        if ((sigadr >= 3021) && (sigadr <= 3023))
        {
            i = sigadr-3021;
            WDFunc::SetLBLTColor(this,QString::number(1000+i), (sigval == 1) ? TABCOLORA1 : ACONFOCLR); //TABCOLORA1
            if (sigval == 0)
                stColor[i] = 1;
        }
        if ((sigadr >= 3024) && (sigadr <= 3026))
        {
            i = sigadr-3024;
            if (sigval == 1)
            {
                stColor[i] = 0;
                WDFunc::SetLBLTColor(this,QString::number(2429+i),REDCOLOR);
            }
            else
            {
                if(!stColor[i])
                    WDFunc::SetLBLTColor(this,QString::number(2429+i),TABCOLORA1);
            }
        }
        if ((sigadr >= 3027) && (sigadr <= 3029))
        {
            i = sigadr-3027;
            if(sigval == 1)
               WDFunc::SetLBLTColor(this,QString::number(2426+i),TABCOLORA1);
            else
            {
               stColor[3+i] = 1;
               WDFunc::SetLBLTColor(this,QString::number(2426+i),ACONFOCLR);
            }
        }
        if ((sigadr >= 3030) && (sigadr <= 3033))
        {
            i = sigadr-3030;
            if(sigval == 1)
            {
               stColor[3+i] = 0;
               WDFunc::SetLBLTColor(this,QString::number(2426+i),REDCOLOR);
            }
            else
            {
               if(!stColor[3+i])
                   WDFunc::SetLBLTColor(this,QString::number(2426+i),TABCOLORA1);
            }
        }
        if(sigadr == 3034)
        {
            if(sigval == 1)
               WDFunc::SetLBLTColor(this,QString::number(2432),TABCOLORA1);
            else
            {
               stColor[6] = 1;
               WDFunc::SetLBLTColor(this,QString::number(2432),ACONFOCLR);
            }
        }
        if(sigadr == 3035)
        {
            if(sigval == 1)
            {
               stColor[6] = 0;
               WDFunc::SetLBLTColor(this,QString::number(2432),REDCOLOR);
            }
            else
            {
               if(!stColor[6])
                   WDFunc::SetLBLTColor(this,QString::number(2432),TABCOLORA1);
            }
        }
    }
}

/*void CheckDialog84::UpdateSponDataWithTime(Parse104::SponSignals *Signal)
{
    //Parse104::SponSignals104 sig = *new Parse104::SponSignals104;
    int i = 0,j;
    for(j=0; j<Signal->SigNumber; j++)
    {
      //sig = *(Signal+j);

        if((Signal->Spon[j].SigAdr == 3011) || (Signal->Spon[j].SigAdr == 3012) || (Signal->Spon[j].SigAdr == 3013))
        {
            i = Signal->Spon[j].SigAdr-3011;
            if(Signal->Spon[j].SigVal == 1)
            {

               WDFunc::SetLBLTColor(this,QString::number(1000+i), TABCOLORA1); //TABCOLORA1
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(1000+i),ACONFOCLR);
            }

        }

        if((Signal->Spon[j].SigAdr == 3014) || (Signal->Spon[j].SigAdr == 3015) || (Signal->Spon[j].SigAdr == 3016))
        {
            i = Signal->Spon[j].SigAdr-3014;
            if(Signal->Spon[j].SigVal == 1)
            {
               WDFunc::SetLBLTColor(this,QString::number(1100+i), TABCOLORA1); //TABCOLORA1
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(1100+i),ACONFOCLR);
            }

        }

        if((Signal->Spon[j].SigAdr == 3018) || (Signal->Spon[j].SigAdr == 3019) || (Signal->Spon[j].SigAdr == 3020))
        {
            i = Signal->Spon[j].SigAdr-3018;
            if(Signal->Spon[j].SigVal == 1)
            {

               WDFunc::SetLBLTColor(this,QString::number(1000+i), TABCOLORA1); //TABCOLORA1
            }
            else
            {
               WDFunc::SetLBLTColor(this,QString::number(1000+i),ACONFOCLR);
            }

        }

        if((Signal->Spon[j].SigAdr == 3021) || (Signal->Spon[j].SigAdr == 3022) || (Signal->Spon[j].SigAdr == 3023))
        {
            i = Signal->Spon[j].SigAdr-3021;
            if(Signal->Spon[j].SigVal == 1)
            {

               WDFunc::SetLBLTColor(this,QString::number(2429+i), TABCOLORA1); //TABCOLORA1
            }
            else
            {
               stColor[i] = 1;
               WDFunc::SetLBLTColor(this,QString::number(2429+i),ACONFOCLR);
            }

        }

        if(Signal->Spon[j].SigAdr == 3024 || Signal->Spon[j].SigAdr == 3025 || Signal->Spon[j].SigAdr == 3026)
        {
            i = Signal->Spon[j].SigAdr-3024;
            if(Signal->Spon[j].SigVal == 1)
            {
               stColor[i] = 0;
               WDFunc::SetLBLTColor(this,QString::number(2429+i),REDCOLOR);
            }
            else
            {
               if(!stColor[i])
               WDFunc::SetLBLTColor(this,QString::number(2429+i),TABCOLORA1);
            }

        }

        if(Signal->Spon[j].SigAdr == 3027 || Signal->Spon[j].SigAdr == 3028 || Signal->Spon[j].SigAdr == 3029)
        {
            i = Signal->Spon[j].SigAdr-3027;
            if(Signal->Spon[j].SigVal == 1)
            {
               WDFunc::SetLBLTColor(this,QString::number(2426+i),TABCOLORA1);
            }
            else
            {
               stColor[3+i] = 1;
               WDFunc::SetLBLTColor(this,QString::number(2426+i),ACONFOCLR);
            }

        }

        if(Signal->Spon[j].SigAdr == 3030 || Signal->Spon[j].SigAdr == 3031 || Signal->Spon[j].SigAdr == 3032)
        {
            i = Signal->Spon[j].SigAdr-3030;
            if(Signal->Spon[j].SigVal == 1)
            {
               stColor[3+i] = 0;
               WDFunc::SetLBLTColor(this,QString::number(2426+i),REDCOLOR);
            }
            else
            {
               if(!stColor[3+i])
               WDFunc::SetLBLTColor(this,QString::number(2426+i),TABCOLORA1);
            }

        }

        if(Signal->Spon[j].SigAdr == 3034)
        {
            if(Signal->Spon[j].SigVal == 1)
            {
               WDFunc::SetLBLTColor(this,QString::number(2432),TABCOLORA1);
            }
            else
            {
               stColor[6] = 1;
               WDFunc::SetLBLTColor(this,QString::number(2432),ACONFOCLR);
            }
        }

        if(Signal->Spon[j].SigAdr == 3035)
        {
            if(Signal->Spon[j].SigVal == 1)
            {
               stColor[6] = 0;
               WDFunc::SetLBLTColor(this,QString::number(2432),REDCOLOR);
            }
            else
            {
               if(!stColor[6])
               WDFunc::SetLBLTColor(this,QString::number(2432),TABCOLORA1);
            }
        }
    }
}
*/
/*void CheckDialog84::UpdateBS104Data(Parse104::BS104Signals* Signal)
{
    Parse104::BS104Signals sig = *new Parse104::BS104Signals;
    sig = *Signal;
//    ModuleBSI::Bsi* Bsi = new ModuleBSI::Bsi;
    int i;
    int adr = 0;
    memcpy(&adr, &(sig.BS.SigAdr[0]), sizeof(sig.BS.SigAdr));

    for(i=0; i<Signal->SigNumber; i++)
    {
        sig = *(Signal+i);

        if(adr == 15)
        {
            ModuleBSI::ModuleBsi.Hth = sig.BS.SigVal;
            emit BsiRefresh();
        }
    }
}*/

void CheckDialog84::UpdateModBusData(QList<ModBus::SignalStruct> Signal)
{

    //ModBusSignal sig = *new ModBusSignal;
    int i = 0;
    for(i=0; i<Signal.size(); ++i)
    {
      //sig = *(Signal+i);
      if((((Signal.at(i).SigAdr >= 1011) && (Signal.at(i).SigAdr <= 1015))) || ((Signal.at(i).SigAdr >= 1111) && (Signal.at(i).SigAdr <= 1115)))
      Ch84->FillBd(this, QString::number((Signal.at(i).SigAdr)+9), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
      else
      Ch84->FillBd(this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal, 3));
    }
}

/*void CheckDialog84::ErrorRead()
{
  //EMessageBox::information(this, "INFO", "Ошибка чтения");
}*/

void CheckDialog84::onModbusStateChanged(ConnectionStates state)
{
    if(state == ConnectionStates::ConnectedState)
        EMessageBox::information(this, "Успешно", "Связь по MODBUS установлена");
//    else
//        EMessageBox::error(this, "Провал", "Подключение отсутствует");
}

void CheckDialog84::SetPredAlarmColor(quint8* PredAlarm)
{

    for(int i = 0; i<18; i++)
    {
        if((i>=0) && (i<3))
        {
           if(PredAlarm[i] == 1)
           WDFunc::SetLBLTColor(this,QString::number(1000+i), TABCOLORA1); //TABCOLORA1
           else
           WDFunc::SetLBLTColor(this,QString::number(1000+i),ACONFOCLR);
        }

        if((i>=3) && (i<6))
        {
           if(PredAlarm[i] == 1)
           WDFunc::SetLBLTColor(this,QString::number(1100+i-3), TABCOLORA1); //TABCOLORA1
           else
           WDFunc::SetLBLTColor(this,QString::number(1100+i-3),ACONFOCLR);
        }

        if((i>=7) && (i<10))
        {
           if(PredAlarm[i] == 1)
           WDFunc::SetLBLTColor(this,QString::number(1000+i-7), TABCOLORA1); //TABCOLORA1
        }

       if((i>=10) && (i<13))
       {
          if(PredAlarm[i] == 1)
          WDFunc::SetLBLTColor(this,QString::number(2429+i-10), TABCOLORA1); //TABCOLORA1
          else
          WDFunc::SetLBLTColor(this,QString::number(2429+i-10),ACONFOCLR);
       }
       else if((i>=13) && (i<16))
       {
           if(PredAlarm[i] == 1)
           WDFunc::SetLBLTColor(this,QString::number(2426+i-13), TABCOLORA1); //TABCOLORA1
           else
           WDFunc::SetLBLTColor(this,QString::number(2426+i-13),ACONFOCLR);
       }
       else if(i == 17)
       {
           if(PredAlarm[i] == 1)
           WDFunc::SetLBLTColor(this,QString::number(2432), TABCOLORA1); //TABCOLORA1
           else
           WDFunc::SetLBLTColor(this,QString::number(2432),ACONFOCLR);
       }
    }

}

void CheckDialog84::SetAlarmColor(quint8* Alarm)
{

    for(int i = 0; i<7; i++)
    {
       if(i<3)
       {
          if(Alarm[i] == 1)
          WDFunc::SetLBLTColor(this,QString::number(2429+i), REDCOLOR);
       }
       else if((i>=3) && (i<6))
       {
           if(Alarm[i] == 1)
           WDFunc::SetLBLTColor(this,QString::number(2426+i-3), REDCOLOR);
       }
       else if(i == 6)
       {
           if(Alarm[i] == 1)
           WDFunc::SetLBLTColor(this,QString::number(2432), REDCOLOR);
       }
    }
}
