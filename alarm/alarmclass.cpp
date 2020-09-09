#include "alarmclass.h"

#include "../usb/commands.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCursor>
#include <QDialog>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStringListModel>
#include <QToolBar>

AlarmClass::AlarmClass(QObject *parent) : QObject(parent)
{
    StAlarm KIV;
    KIV.warnCounts = 18;
    KIV.avarCounts = 7;
    KIV.BdNumbers = 11;
    KIV.AdrAlarm = 3011;
    KIV.warns = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0 };
    KIV.avars = { 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1 };

    StAlarm KTF;
    KTF.warnCounts = 13;
    KTF.avarCounts = 2;
    KTF.BdNumbers = 16;
    KTF.AdrAlarm = 5011;
    KTF.warns = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 };
    KTF.avars = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 };

    MapAlarm[MTYPE_KIV] = KIV;
    MapAlarm[MTYPE_KTF] = KTF;
}

void AlarmClass::UpdateAlarmUSB()
{
    BdAlarm signalling;
    int i = 0;
    if (Board::GetInstance()->interfaceType() == Board::InterfaceType::USB)
    {
        if (Commands::GetBd(MapAlarm[Board::GetInstance()->type()].BdNumbers, &signalling, sizeof(BdAlarm)) == NOERROR)
        {
            for (i = 0; i < MapAlarm[Board::GetInstance()->type()].warnCounts; ++i)
            {
                bool warn = (signalling.Warn & (0x00000001 << i));
                WarnAlarmEvents.append(warn);
            }

            for (i = 0; i < MapAlarm[Board::GetInstance()->type()].avarCounts; ++i)
            {
                bool avar = (signalling.Alarm & (0x00000001 << i));
                AvarAlarmEvents.append(avar);
            }

            emit SetWarnAlarmColor(WarnAlarmEvents);
            emit SetAlarmColor(AvarAlarmEvents);
        }
    }
    if (Commands::GetBsi(ModuleBSI::ModuleBsi) == NOERROR)
        emit SetFirstButton();
}

void AlarmClass::UpdateAlarmModBus(ModBus::Coils Signal)
{
    int i = 0;
    int ccount = 0;

    for (i = 0; i < Signal.countBytes; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (ccount <= MapAlarm[Board::GetInstance()->type()].warns.size())
            {
                bool alarm = (Signal.Bytes[i] & (0x00000001 << j));
                if (MapAlarm[Board::GetInstance()->type()].warns.at(ccount))
                    WarnAlarmEvents.append(alarm);
                else if (MapAlarm[Board::GetInstance()->type()].avars.at(ccount))
                    AvarAlarmEvents.append(alarm);
                ccount++;
            }
        }
    }
    emit SetWarnAlarmColor(WarnAlarmEvents);
    emit SetAlarmColor(AvarAlarmEvents);
    emit SetFirstButton();
}

void AlarmClass::UpdateAlarm104(IEC104Thread::SponSignals *Signal)
{
    for (int i = 0, count = 0; i < Signal->SigNumber; i++)
    {
        quint8 sigval = Signal->Spon[i].SigVal;
        if (!(sigval & 0x80))
        {
            quint32 sigadr = Signal->Spon[i].SigAdr;
            bool alarm = (sigval & 0x00000001) ? 1 : 0;
            quint32 AdrAlarm = MapAlarm[Board::GetInstance()->type()].AdrAlarm;
            int WarnsSize = MapAlarm[Board::GetInstance()->type()].warns.size();
            if ((AdrAlarm <= sigadr) && (sigadr <= AdrAlarm + WarnsSize))
                if (MapAlarm[Board::GetInstance()->type()].warns.at(count))
                    WarnAlarmEvents.append(alarm);
                else if (MapAlarm[Board::GetInstance()->type()].avars.at(count))
                    AvarAlarmEvents.append(alarm);
            count++;
        }
    }

    emit SetWarnAlarmColor(WarnAlarmEvents);
    emit SetAlarmColor(AvarAlarmEvents);
    emit SetFirstButton();
}
