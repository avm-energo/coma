
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
#include "predalarm84.h"

PredAlarm84::PredAlarm84(QWidget *parent):
    AbstractAlarm(parent)
{
   PredAlarmState();
}


void PredAlarm84::PredAlarmState()
{
     QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    const QStringList events = QStringList() << "Отсутствует сигнал напряжения фазы A                   "
                                             << "Отсутствует сигнал напряжения фазы B                   "
                                             << "Отсутствует сигнал напряжения фазы С                   "
                                             << "Отсутствует ток ввода фазы А (ток меньше 2мА)          "
                                             << "Отсутствует ток ввода фазы B (ток меньше 2мА)          "
                                             << "Отсутствует ток ввода фазы C (ток меньше 2мА)          "
                                             << "Не заданы начальные значения                           "
                                             << "Низкое напряжение фазы A                               "
                                             << "Низкое напряжение фазы B                               "
                                             << "Низкое напряжение фазы C                               "
                                             << "Сигнализация по приращению тангенса дельта ввода фазы А"
                                             << "Сигнализация по приращению тангенса дельта ввода фазы B"
                                             << "Сигнализация по приращению тангенса дельта ввода фазы C"
                                             << "Сигнализация по приращению C ввода фазы А              "
                                             << "Сигнализация по приращению C ввода фазы B              "
                                             << "Сигнализация по приращению C ввода фазы C              "
                                             << "Не заданы паспортные значения                          "
                                             << "Сигнализация по повышенному небалансу токов            ";
    QWidget *w = new QWidget;
    Wpred = w;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    for (int i = 0; i < 13; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3011+i), (PredAlarmEvents[i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }
    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3027+i), (PredAlarmEvents[13+i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(13+i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "3033", (PredAlarmEvents[16]) ? pmred : pmgrn));
    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(16), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "3034", (PredAlarmEvents[17]) ? pmred : pmgrn));
    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(17), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    setLayout(lyout);
}

void PredAlarm84::AlarmState()
{
}
void PredAlarm84::AvarState()
{
}
void PredAlarm84::UpdateUSB()
{
}
void PredAlarm84::USBSetAlarms()
{
}
void PredAlarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);

}
void PredAlarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void PredAlarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void PredAlarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}

