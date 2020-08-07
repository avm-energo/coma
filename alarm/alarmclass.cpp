#include "alarmclass.h"

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

AlarmClass::AlarmClass(QObject *parent) : QObject(parent)
{
    warnCounts[0xA284]=18;
    warnCounts[0xA287]=15;

    avarCounts[0xA284]=7;
    avarCounts[0xA287]=2;

    BdNumbers[0xA284]=11;
    BdNumbers[0xA287]=16;

}

void AlarmClass::UpdateAlarmUSB()
{
    int i = 0, predalarmcount = 0, alarmcount = 0;
    BdAlarm signalling;

    quint32 MType=(MTypeB<<8)+MTypeM;

    if (Commands::GetBd(BdNumbers[MType], &signalling, sizeof(BdAlarm)) == NOERROR)
    {
        bool warn=(signalling.Warn & (0x00000001 << i));
        bool alarm=(signalling.Alarm & (0x00000001 << i));
        for(i=0; i<warnCounts[MType]; ++i)
        {
           WarnAlarmEvents.append(warn);
           if(warn)
               ++predalarmcount;

        }

        for(i=0; i<warnCounts[MType]; ++i)
        {
            AvarAlarmEvents.append(alarm);
            if(alarm)
                ++alarmcount;
        }

    }
    emit SetWarnAlarmColor(WarnAlarmEvents);
    emit SetAlarmColor(AvarAlarmEvents);

}
