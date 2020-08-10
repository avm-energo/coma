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
    warnCounts[MTYPE_KIV]=18;
    warnCounts[MTYPE_KTF]=13;

    avarCounts[MTYPE_KIV]=7;
    avarCounts[MTYPE_KTF]=2;

    BdNumbers[MTYPE_KIV]=11;
    BdNumbers[MTYPE_KTF]=16;

}

void AlarmClass::UpdateAlarmUSB()
{

    BdAlarm signalling;
    int i = 0;
    quint32 MType=MTypeB+MTypeM;

    if (Commands::GetBd(BdNumbers[MType], &signalling, sizeof(BdAlarm)) == NOERROR)
    {
        for(i=0; i<warnCounts[MType]; ++i)
        {
           bool warn=(signalling.Warn & (0x00000001 << i));
           WarnAlarmEvents.append(warn);
//           if(warn)
//               ++warnalarmcount;
        }

        for(i=0; i<warnCounts[MType]; ++i)
        {
            bool avar=(signalling.Alarm & (0x00000001 << i));
            AvarAlarmEvents.append(avar);
//            if(avar)
//                ++alarmcount;
        }
        emit SetWarnAlarmColor(WarnAlarmEvents);
        emit SetAlarmColor(AvarAlarmEvents);
        if (Commands::GetBsi(ModuleBSI::ModuleBsi) == NOERROR)
        emit SetFirstButton();
    }




}
