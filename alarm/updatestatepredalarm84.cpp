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
#include "updatestatepredalarm84.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"

UpdateStatePredAlarm84::UpdateStatePredAlarm84(QWidget *parent):
    AbstractAlarm(parent)
{


}

void UpdateStatePredAlarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    int i = 0, predalarmcount = 0, alarmcount = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    for(i=0; i<Signal->SigNumber; i++)
    {
        quint8 sigval = Signal->Spon[i].SigVal;
        if(!(sigval & 0x80))
        {
            quint32 sigadr = Signal->Spon[i].SigAdr;
            int pmidx = (sigval & 0x00000001) ? 1 : 0;

            if((sigadr >= 3011) && (sigadr < 3024))
            PredAlarmEvents[sigadr - 3011] = pmidx;
            else if((sigadr >= 3024) && (sigadr < 3027))
            AvarAlarmEvents[sigadr - 3024] = pmidx;
            else if((sigadr >= 3027) && (sigadr < 3030))
            PredAlarmEvents[sigadr - 3014] = pmidx;
            else if((sigadr >= 3030) && (sigadr < 3033))
            AvarAlarmEvents[sigadr - 3027] = pmidx;
            else if(sigadr == 3033)
            PredAlarmEvents[16] = pmidx;
            else if(sigadr == 3034)
            PredAlarmEvents[17] = pmidx;
            else if(sigadr == 3035)
            AvarAlarmEvents[6] = pmidx;
        }
    }
    for(i=0; i<18; i++)
    {
        if(PredAlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, "951", pmred);
           predalarmcount++;
        }
    }
    if(predalarmcount == 0)
        WDFunc::SetLBLImage(this,  "951", pmgrn);
    for(i=0; i<7; i++)
    {
        if(AvarAlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, "952", pmred);
           alarmcount++;
        }
    }
    if(alarmcount == 0)
        WDFunc::SetLBLImage(this, "952", pmgrn);
}

void UpdateStatePredAlarm84::AlarmState()
{
}
void UpdateStatePredAlarm84::AvarState()
{
}
void UpdateStatePredAlarm84::UpdateUSB()
{
}
void UpdateStatePredAlarm84::USBSetAlarms()
{
}
void UpdateStatePredAlarm84::PredAlarmState()
{

}
void UpdateStatePredAlarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void UpdateStatePredAlarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void UpdateStatePredAlarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}







