#include "predalarmktf.h"
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


PredAlarmKTF::PredAlarmKTF(QWidget *parent):
    AbstractAlarm(parent)
{
  PredAlarmState();
}

void PredAlarmKTF::PredAlarmState()
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
                                             << "Отсутствует сигнал тока  фазы А                        "
                                             << "Отсутствует сигнал тока  фазы B                        "
                                             << "Отсутствует сигнал тока  фазы C                        "
                                             << "Перегрузка по току фазы A                              "
                                             << "Перегрузка по току фазы В                              "
                                             << "Перегрузка по току фазы С                              "
                                             << "Машина включена                                        "
                                             << "Сигнализация по опасному превышению температуры обмотки"
                                             << "Неисправны все датчики температуры обмотки             "
                                             << "Сигнализация по опасному уровню пускового тока         ";
    QWidget *w = new QWidget;
    Wpred = w;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);

    for (int i = 0; i < 12; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(5011+i), (PredAlarmEvents[i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }


    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "5024", (PredAlarmEvents[13]) ? pmred : pmgrn));
    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(13), "", "", ""), 1);
    vlayout->addLayout(hlyout);



    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->show();

}

void PredAlarmKTF::AlarmState()
{
}
void PredAlarmKTF::AvarState()
{
}

void PredAlarmKTF::UpdateUSB()
{
}

void PredAlarmKTF::USBSetAlarms()
{
}

void PredAlarmKTF::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);

}
void PredAlarmKTF::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void PredAlarmKTF::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void PredAlarmKTF::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}

