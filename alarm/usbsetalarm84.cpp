
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
#include "usbsetalarm84.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"

USBSetAlarm84::USBSetAlarm84(QWidget *parent):
    AbstractAlarm(parent)
{

}

void USBSetAlarm84::USBSetAlarms()
{
    if (MainInterface == I_USB)
    {
        int i = 0;
        QPixmap *pmgrn = new QPixmap("images/greenc.png");
        QPixmap *pmred = new QPixmap("images/redc.png");
        Bd11 Bd_block11;

        if (Commands::GetBd(11, &Bd_block11, sizeof(Bd11)) == NOERROR)
        {
            if(Wpred != nullptr)
            {
                for(i=0; i<13; i++)
                {
                    quint32 alarm = Bd_block11.predAlarm & ((quint32)0x00000001 << i);
                    WDFunc::SetLBLImage(Walarm, (QString::number(3011+i)), (alarm) ? pmred : pmgrn);
                }
                for(i=13; i<16; i++)
                {
                    quint32 alarm = Bd_block11.predAlarm & ((quint32)0x00000001 << i);
                    WDFunc::SetLBLImage(Walarm, (QString::number(3027+i-13)), (alarm) ? pmred : pmgrn);
                }
                quint32 alarm = Bd_block11.predAlarm & ((quint32)0x00000001 << 16);
                WDFunc::SetLBLImage(Walarm, "3033", (alarm) ? pmred : pmgrn);
                alarm = Bd_block11.predAlarm & ((quint32)0x00000001 << 17);
                WDFunc::SetLBLImage(Walarm, "3034", (alarm) ? pmred : pmgrn);
            }

            if(Walarm != nullptr)
            {
                for(i=0; i<3; i++)
                {
                    quint32 alarm = Bd_block11.alarm & ((quint32)0x00000001 << i);
                    WDFunc::SetLBLImage(Walarm, (QString::number(3024+i)), (alarm) ? pmred : pmgrn);
                }

                for(i=3; i<6; i++)
                {
                    quint32 alarm = Bd_block11.alarm & ((quint32)0x00000001 << i);
                    WDFunc::SetLBLImage(Walarm, (QString::number(3030+i-3)), (alarm) ? pmred : pmgrn);
                }

                quint32 alarm = Bd_block11.alarm & ((quint32)0x00000001 << 6);
                if(Bd_block11.alarm & ((quint32)0x00000001 << 6))
                    WDFunc::SetLBLImage(Walarm, "3035", (alarm) ? pmred : pmgrn);
            }
        }
    }
}
void USBSetAlarm84::AlarmState()
{
}
void USBSetAlarm84::AvarState()
{
}
void USBSetAlarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void USBSetAlarm84::UpdateUSB()
{
}
void USBSetAlarm84::PredAlarmState()
{

}
void USBSetAlarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void USBSetAlarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void USBSetAlarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
