#include "avaralarmktf.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"

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

AvarAlarmKTF::AvarAlarmKTF(AlarmClass *alarm, QDialog *parent) : AbstractAlarm(parent)
{
    Alarm = alarm;
    AvarState();
}

void AvarAlarmKTF::AvarState()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    QStringList events = QStringList() << "Аварийное сообщение по недопустимому превышению температуры обмотки"
                                       << "Аварийное сообщение по недопустимому уровню пускового тока         ";

    QWidget *w = new QWidget;

    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    for (int i = 0; i < Alarm->MapAlarm[MTYPE_KTF].avarCounts; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), this, SLOT(close()));
    lyout->addWidget(pb, 0);
    setLayout(lyout);
}

void AvarAlarmKTF::AlarmState()
{
}
void AvarAlarmKTF::WarnAlarmState()
{
}

void AvarAlarmKTF::Update(QList<bool> states)
{
    int i;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    for (i = 0; i < Alarm->MapAlarm[MTYPE_KTF].avarCounts; i++)
    {
        quint32 alarm = states[i];
        WDFunc::SetLBLImage(this, (QString::number(i)), (alarm) ? pmred : pmgrn);
    }
}
