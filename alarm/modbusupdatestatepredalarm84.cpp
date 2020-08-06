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
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"
#include "modbusupdatestatepredalarm84.h"

ModbusUpdateStatePredAlarm84::ModbusUpdateStatePredAlarm84(QWidget *parent):
    AbstractAlarm(parent)
{

}

void ModbusUpdateStatePredAlarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    int i = 0, predalarmcount = 0, alarmcount = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    for(i=0; i<Signal.countBytes; i++)
    {
        if(i == 3)
        {
            quint8 signal = ((Signal.Bytes[i] & (0x00000001)) ? 1 : 0);
            AvarAlarmEvents[6] = signal;
        }
        if(i == 2)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);

                if(j<3)
                    PredAlarmEvents[13+j] = signal;
                else if(j>=3 && j<6)
                    AvarAlarmEvents[j] = signal;
                if(j==6)
                    PredAlarmEvents[16] = signal;
                if(j==7)
                    PredAlarmEvents[17] = signal;
            }
        }
        else if(i == 0)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);
                PredAlarmEvents[j] = signal;
            }
        }
        else if(i == 1)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);

                if(j<5)
                    PredAlarmEvents[8+j] = signal;
                if(j >= 5)
                    AvarAlarmEvents[j-5] = signal;
            }
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

    emit SetPredAlarmColor(PredAlarmEvents);

    for(i=0; i<7; i++)
    {
        if(AvarAlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, "952", pmred);
           alarmcount++;
        }
    }
    if(alarmcount == 0)
        WDFunc::SetLBLImage(this,  "952", pmgrn);
    emit SetAlarmColor(AvarAlarmEvents);
}

void ModbusUpdateStatePredAlarm84::AlarmState()
{
}
void ModbusUpdateStatePredAlarm84::PredAlarmState()
{
}
void ModbusUpdateStatePredAlarm84::UpdateUSB()
{
}
void ModbusUpdateStatePredAlarm84::USBSetAlarms()
{
}
void ModbusUpdateStatePredAlarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);

}
void ModbusUpdateStatePredAlarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void ModbusUpdateStatePredAlarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void ModbusUpdateStatePredAlarm84::AvarState()
{
}





