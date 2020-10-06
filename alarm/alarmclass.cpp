#include "alarmclass.h"

#include "../gen/board.h"
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
    //    StAlarm KIV;
    //    KIV.warnCounts = 18;
    //    KIV.avarCounts = 7;
    //    KIV.BdNumbers = 11;
    //    KIV.AdrAlarm = 3011;
    //    KIV.warns = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0 };
    //    KIV.avars = { 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1 };

    //    StAlarm KTF;
    //    KTF.warnCounts = 13;
    //    KTF.avarCounts = 2;
    //    KTF.BdNumbers = 16;
    //    KTF.AdrAlarm = 5011;
    //    KTF.warns = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 };
    //    KTF.avars = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 };

    //    MapAlarm[Board::DeviceModel::KIV] = KIV;
    //    MapAlarm[Board::DeviceModel::KTF] = KTF;
}

void AlarmClass::UpdateAlarmUSB()
{
    bool warns = false, alarms = false;
    Warn *w = m_Module->getWarn();
    Alarm *a = m_Module->getAlarm();
    //    WarnAlarmEvents.clear();
    //    AvarAlarmEvents.clear();
    // IndicatorEvents.clear();
    BdAlarm signalling;
    int i = 0;
    bool ind;
    if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        //        if (Commands::GetBd(MapAlarm[Board::GetInstance().type()].BdNumbers, &signalling, sizeof(BdAlarm))
        //            == Error::Msg::NoError)
        if (Commands::GetBd(m_Module->getAlarm()->m_alarmBdNum, &signalling, sizeof(BdAlarm)) == Error::Msg::NoError)
        {
            //            for (i = 0; i < MapAlarm[Board::GetInstance().type()].warnCounts; ++i)
            for (i = 0; i < m_Module->getWarn()->realWarnSize(); ++i)
            {
                bool signal = signalling.Warn & (0x00000001 << i);
                if (Board::GetInstance().type() == Board::DeviceModel::KTF)
                {
                    if (i != 9)
                    {
                        //                        bool warn = (signalling.Warn & (0x00000001 << i));
                        a->updatePixmap(signal, i);
                        //                        WarnAlarmEvents.append(warn);
                    }
                    else
                        ind = signal;
                }
                else
                {
                    //                    bool warn = (signalling.Warn & (0x00000001 << i));
                    a->updatePixmap(signal, i);
                    //                    WarnAlarmEvents.append(warn);
                }
                warns |= signal;
            }

            for (i = 0; i < m_Module->getAlarm()->realAlarmSize(); ++i)
            //            for (i = 0; i < MapAlarm[Board::GetInstance().type()].avarCounts; ++i)
            {
                bool signal = signalling.Warn & (0x00000001 << i);
                w->updatePixmap(signal, i);
                //                bool avar = (signalling.Alarm & (0x00000001 << i));
                //                AvarAlarmEvents.append(avar);
                warns |= signal;
            }

            emit setWarnColor(warns);
            emit setAlarmColor(alarms);
            /*            emit SetWarnAlarmColor(WarnAlarmEvents);
                        emit SetAlarmColor(AvarAlarmEvents); */
            //            m_Module->getAlarm()->Update(alarmBitSet);
            //            m_Module->getWarn()->Update(warnBitSet);
            if (Board::GetInstance().type() == Board::DeviceModel::KTF)
                emit SetIndicator(ind);
        }
    }
    if (Commands::GetBsi(ModuleBSI::ModuleBsi) == Error::Msg::NoError)
        emit SetFirstButton();
}

void AlarmClass::UpdateAlarmModBus(ModBus::Coils Signal)
{
    Warn *w = m_Module->getWarn();
    Alarm *a = m_Module->getAlarm();
    //    WarnAlarmEvents.clear();
    //    AvarAlarmEvents.clear();
    int i = 0;
    int wcount = 0; // warning counter
    int acount = 0; // alarm counter

    //    Q_ASSERT(MapAlarm.contains(Board::GetInstance().type()));
    int maxcountbytes = std::min(4, Signal.countBytes); // 32 bits maximum
    for (i = 0; i < maxcountbytes; ++i)
    {
        for (int j = 0; j < 8; j++)
        {
            //            if (ccount < MapAlarm[Board::GetInstance().type()].warns.size())
            //            {
            if (w->m_warnFlags[wcount])
                w->updatePixmap(Signal.Bytes[i] & (0x00000001 << j), wcount++);
            if (a->m_alarmFlags[acount])
                a->updatePixmap(Signal.Bytes[i] & (0x00000001 << j), acount++);

            //                bool alarm = (Signal.Bytes[i] & (0x00000001 << j));
            //                if (MapAlarm[Board::GetInstance().type()].warns.at(ccount))
            //                    WarnAlarmEvents.append(alarm);
            //                else if (MapAlarm[Board::GetInstance().type()].avars.at(ccount))
            //                    AvarAlarmEvents.append(alarm);
            //                ccount++;
            //            }
        }
    }
    //    emit SetWarnAlarmColor(WarnAlarmEvents);
    //    emit SetAlarmColor(AvarAlarmEvents);
    emit SetFirstButton();
}

void AlarmClass::UpdateAlarm104(IEC104Thread::SponSignals *Signal)
{
    try
    {
        Warn *w = m_Module->getWarn();
        Alarm *a = m_Module->getAlarm();
        //        WarnAlarmEvents.clear();
        //        AvarAlarmEvents.clear();
        int wcount = 0; // warning counter
        int acount = 0; // alarm counter
        quint32 minAddress = w->m_startWarnAddress;
        quint32 maxAddress = w->m_startWarnAddress + 32; // only 32 bits
        for (int i = 0; i < Signal->SigNumber; i++)
        {
            quint8 sigval = Signal->Spon[i].SigVal;
            if (!(sigval & 0x80))
            {
                quint32 sigadr = Signal->Spon[i].SigAdr;
                if ((minAddress <= sigadr) && (sigadr <= maxAddress))
                {
                    if (w->m_warnFlags[wcount])
                        w->updatePixmap(sigval & 0x00000001, wcount++);
                    if (a->m_alarmFlags[acount])
                        a->updatePixmap(sigval & 0x00000001, acount++);
                }
                //                bool alarm = (sigval & 0x00000001) ? 1 : 0;
                //                quint32 AdrAlarm = MapAlarm[Board::GetInstance().type()].AdrAlarm;
                //                int WarnsSize = MapAlarm[Board::GetInstance().type()].warns.size();
                //                if ((AdrAlarm <= sigadr) && (sigadr <= AdrAlarm + WarnsSize))
                //                {
                //                    if (MapAlarm[Board::GetInstance().type()].warns.at(i))
                //                        WarnAlarmEvents.append(alarm);
                //                    else if (MapAlarm[Board::GetInstance().type()].avars.at(i))
                //                        AvarAlarmEvents.append(alarm);
                //                }
            }
        }

        //        emit SetWarnAlarmColor(WarnAlarmEvents);
        //        emit SetAlarmColor(AvarAlarmEvents);
        emit SetFirstButton();
    }
    catch (const std::exception &ex)
    {
        QString str = "Exception: ";
        str.append(ex.what());
        ERMSG(str);
    }
    catch (...)
    {
        ERMSG("Unhandled exception");
    }
}
