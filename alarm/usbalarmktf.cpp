#include "usbalarmktf.h"
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


#include "../gen/colors.h"
#include "../widgets/wd_func.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"

USBAlarmKTF::USBAlarmKTF(QWidget *parent):
    AbstractAlarm(parent)
{
    if (MainInterface == I_USB)
    {
        int i = 0, predalarmcount = 0, alarmcount = 0;
        QPixmap *pmgrn = new QPixmap("images/greenc.png");
        QPixmap *pmylw = new QPixmap("images/yellowc.png");
        QPixmap *pmred = new QPixmap("images/redc.png");
        Bd16 Bd_block16;

        INFOMSG("UpdateUSB()");
//        if (CheckB != nullptr)
//            CheckB->USBUpdate();

        if (Commands::GetBd(16, &Bd_block16, sizeof(Bd16)) == NOERROR)
        {
            for(i=0; i<13; i++)
            {
               if(Bd_block16.Warn & (0x00000001 << i))
               {
                   PredAlarmEventsKTF[i] = 1;
                   ++predalarmcount;
               }
               else
                   PredAlarmEventsKTF[i] = 0;
            }

            for(i=0; i<2; i++)
            {
               if(Bd_block16.Alarm & (0x00000001 << i))
               {
                   AvarAlarmEventsKTF[i] = 1;
                   ++alarmcount;
               }
               else
                   AvarAlarmEventsKTF[i] = 0;
            }
            WDFunc::SetLBLImage(this, "951", (predalarmcount == 0) ? pmgrn : pmylw);
            WDFunc::SetLBLImage(this, "952", (alarmcount == 0) ? pmgrn : pmred);
            emit SetPredAlarmColor(PredAlarmEventsKTF);
            emit SetAlarmColor(AvarAlarmEventsKTF);
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

void USBAlarmKTF::AlarmState()
{
}
void USBAlarmKTF::AvarState()
{
}
void USBAlarmKTF::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void USBAlarmKTF::USBSetAlarms()
{
}
void USBAlarmKTF::UpdateUSB()
{
}
void USBAlarmKTF::PredAlarmState()
{

}
void USBAlarmKTF::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void USBAlarmKTF::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void USBAlarmKTF::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
