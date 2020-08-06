#include "usbalarm84.h"
#include <QLabel>
#include <QProgressBar>
#include <QDir>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QDialog>
#include <QCursor>
#include <QSplashScreen>
#include <QApplication>
#include <QSettings>
#include <QGroupBox>
#include <QStandardPaths>
#include <QStringListModel>
#include <QBoxLayout>
#include <QPushButton>


#include "abstractalarm.h"
#include "../gen/colors.h"
#include "../widgets/wd_func.h"
#include "usbalarm84.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"

UsbAlarm84::UsbAlarm84(QWidget *parent):
    AbstractAlarm(parent)
{

}

void UsbAlarm84::UpdateUSB()
{
    if (MainInterface == I_USB)
    {
        int i = 0, predalarmcount = 0, alarmcount = 0;
        QPixmap *pmgrn = new QPixmap("images/greenc.png");
        QPixmap *pmylw = new QPixmap("images/yellowc.png");
        QPixmap *pmred = new QPixmap("images/redc.png");
        Bd11 Bd_block11;

        INFOMSG("UpdateUSB()");
//        if (CheckB != nullptr)
//            CheckB->USBUpdate();

        if (Commands::GetBd(11, &Bd_block11, sizeof(Bd11)) == NOERROR)
        {
            for(i=0; i<18; i++)
            {
               if(Bd_block11.predAlarm & (0x00000001 << i))
               {
                   PredAlarmEvents[i] = 1;
                   ++predalarmcount;
               }
               else
                   PredAlarmEvents[i] = 0;
            }

            for(i=0; i<7; i++)
            {
               if(Bd_block11.alarm & (0x00000001 << i))
               {
                   AvarAlarmEvents[i] = 1;
                   ++alarmcount;
               }
               else
                   AvarAlarmEvents[i] = 0;
            }
            WDFunc::SetLBLImage(this, "951", (predalarmcount == 0) ? pmgrn : pmylw);
            WDFunc::SetLBLImage(this, "952", (alarmcount == 0) ? pmgrn : pmred);
            emit SetPredAlarmColor(PredAlarmEvents);
            emit SetAlarmColor(AvarAlarmEvents);
        }
        if (Commands::GetBsi(ModuleBSI::ModuleBsi) == NOERROR)
        {
            if (ModuleBSI::ModuleBsi.Hth & PredBSIMask)
                WDFunc::SetLBLImage(this, "950", pmylw);
            else if (ModuleBSI::ModuleBsi.Hth & AvarBSIMask)
                WDFunc::SetLBLImage(this, "950", pmred);
            else
                WDFunc::SetLBLImage(this, "950", pmgrn);
        }
    }
}

void UsbAlarm84::AlarmState()
{
}
void UsbAlarm84::AvarState()
{
}
void UsbAlarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void UsbAlarm84::USBSetAlarms()
{
}
void UsbAlarm84::PredAlarmState()
{

}
void UsbAlarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void UsbAlarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void UsbAlarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
