#include "usbsetalarmktf.h"

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

USBSetAlarmKTF::USBSetAlarmKTF(QWidget *parent):
    AbstractAlarm(parent)
{

    if (MainInterface == I_USB)
    {
        int i = 0;
        QPixmap *pmgrn = new QPixmap("images/greenc.png");
        QPixmap *pmred = new QPixmap("images/redc.png");
        Bd16 Bd_block16;

        INFOMSG("USBSetAlarms()");
        if (Commands::GetBd(16, &Bd_block16, sizeof(Bd11)) == NOERROR)
        {
            if(Wpred != nullptr)
            {
                for(i=0; i<12; i++)
                {
                    quint32 alarm = Bd_block16.Warn & ((quint32)0x00000001 << i);
                    WDFunc::SetLBLImage(Walarm, (QString::number(3011+i)), (alarm) ? pmred : pmgrn);
                }

                quint32 alarm = Bd_block16.Warn & ((quint32)0x00000001 << 13);
                WDFunc::SetLBLImage(Walarm, "5024", (alarm) ? pmred : pmgrn);

            }
            if(Walarm != nullptr)
            {

                quint32 alarm = Bd_block16.Alarm & ((quint32)0x00000001 << 12);
                if(Bd_block16.Alarm & ((quint32)0x00000001 << 12))
                    WDFunc::SetLBLImage(Walarm, "5023", (alarm) ? pmred : pmgrn);

                alarm = Bd_block16.Alarm & ((quint32)0x00000001 << 14);
                if(Bd_block16.Alarm & ((quint32)0x00000001 << 14))
                    WDFunc::SetLBLImage(Walarm, "5025", (alarm) ? pmred : pmgrn);
            }
        }
    }


}

void USBSetAlarmKTF::AlarmState()
{
}
void USBSetAlarmKTF::AvarState()
{
}
void USBSetAlarmKTF::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void USBSetAlarmKTF::UpdateUSB()
{
}
void USBSetAlarmKTF::USBSetAlarms()
{
}
void USBSetAlarmKTF::PredAlarmState()
{

}
void USBSetAlarmKTF::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void USBSetAlarmKTF::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void USBSetAlarmKTF::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
