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
#include "modbusupdatepredalarm84.h"

ModBusUpdatePredAlarm84::ModBusUpdatePredAlarm84(QWidget *parent):
    AbstractAlarm(parent)
{

}

void ModBusUpdatePredAlarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    int i = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmgrn, pmred};

    for(i=0; i<Signal.countBytes; i++)
    {
        if(i == 3)
        {
           quint8 signal = ((Signal.Bytes[i] & (0x00000001)) ? 1 : 0);
           WDFunc::SetLBLImage(Walarm, (QString::number(3035)), pm[signal]);
        }
        if(i == 2)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);

                if(j<3)
                WDFunc::SetLBLImage(Wpred, (QString::number(3027+j)), pm[signal]);
                else if(j>=3 && j<6)
                WDFunc::SetLBLImage(Walarm, (QString::number(3030+j-3)), pm[signal]);
                if(j==6)
                WDFunc::SetLBLImage(Wpred, "3033", pm[signal]);
                if(j==7)
                WDFunc::SetLBLImage(Wpred, "3034", pm[signal]);

            }
        }
        else if(i == 0)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);
                WDFunc::SetLBLImage(Wpred, (QString::number(3011+j)), pm[signal]);
            }
        }
        else if(i == 1)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);

                if(j<5)
                WDFunc::SetLBLImage(Wpred, (QString::number(3019+j)), pm[signal]);
                if(j >= 5)
                WDFunc::SetLBLImage(Walarm, (QString::number(3024+j-5)), pm[signal]);
            }
        }
    }
}


void ModBusUpdatePredAlarm84::AlarmState()
{
}
void ModBusUpdatePredAlarm84::PredAlarmState()
{
}
void ModBusUpdatePredAlarm84::UpdateUSB()
{
}
void ModBusUpdatePredAlarm84::Update()
{
}
void ModBusUpdatePredAlarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);

}
void ModBusUpdatePredAlarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void ModBusUpdatePredAlarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void ModBusUpdatePredAlarm84::AvarState()
{
}


