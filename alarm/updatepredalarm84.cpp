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
#include "updatepredalarm84.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"

UpdatePredAlarm84::UpdatePredAlarm84(QWidget *parent):
    AbstractAlarm(parent)
{


}

void UpdatePredAlarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    int i = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmylw = new QPixmap("images/yellowc.png");
    QPixmap *pm[2] = {pmgrn, pmylw};

    for(i=0; i<Signal->SigNumber; i++)
    {
        quint32 sigadr = Signal->Spon[i].SigAdr;
        quint8 sigval = Signal->Spon[i].SigVal;
        if(!(sigval & 0x80))
        {
            int pmidx = (sigval & 0x00000001) ? 1 : 0;
            if((((sigadr >= 3024) && (sigadr < 3027))) || ((sigadr >= 3030) && (sigadr < 3033)) || (sigadr == 3035))
                WDFunc::SetLBLImage(Walarm, (QString::number(sigadr)), pm[pmidx]);
            else
                WDFunc::SetLBLImage(Wpred, (QString::number(sigadr)), pm[pmidx]);
       }
    }
}

void UpdatePredAlarm84::AlarmState()
{
}
void UpdatePredAlarm84::AvarState()
{
}
void UpdatePredAlarm84::UpdateUSB()
{
}
void UpdatePredAlarm84::USBSetAlarms()
{
}
void UpdatePredAlarm84::PredAlarmState()
{

}
void UpdatePredAlarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void UpdatePredAlarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void UpdatePredAlarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}

