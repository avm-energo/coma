#include "alarmwidget.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCursor>
//#include <QDialog>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
//#include <QPushButton>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStringListModel>
#include <QToolBar>

AlarmWidget::AlarmWidget(AlarmClass *alarm, QWidget *parent) : QWidget(parent)
{
    Alarm = alarm;

    QMenu *menu = new QMenu;
    QString tmps = "QMenuBar {background-color: " + QString(MAINWINCLR)
        + ";}"
          "QMenuBar::item {background-color: "
        + QString(MAINWINCLR) + ";}";
    menu->setStyleSheet(tmps);
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QHBoxLayout *hlyout2 = new QHBoxLayout;
    QStringList Discription = QStringList() << "Состояние устройства"
                                            << "Предупредительная сигнализация"
                                            << "Аварийная сигнализация";
    setStyleSheet("QComa {background-color: " + QString(MAINWINCLR) + ";}");
    QPixmap *pmgrn = new QPixmap("images/greenc.png");

    QPushButton *pb = new QPushButton("Состояние устройства");
    pb->setMinimumSize(QSize(230, 30));
    connect(pb, SIGNAL(clicked()), this, SIGNAL(AlarmButtonPressed()));
    QGroupBox *gb = new QGroupBox("");
    hlyout->addWidget(pb, Qt::AlignRight);
    hlyout->addWidget(WDFunc::NewLBL(this, "", "", "950", pmgrn), 1);
    WDFunc::SetVisible(this, "950", false);
    gb->setLayout(hlyout);
    hlyout2->addWidget(gb);

    gb = new QGroupBox("");
    hlyout = new QHBoxLayout;
    pb = new QPushButton("Предупредительная сигнализация");
    pb->setMinimumSize(QSize(230, 30));
    connect(pb, SIGNAL(clicked()), this, SIGNAL(ModuleWarnButtonPressed()));
    hlyout->addWidget(pb, Qt::AlignRight);
    hlyout->addWidget(WDFunc::NewLBL(this, "", "", "951", pmgrn), 1);
    WDFunc::SetVisible(this, "951", false);
    gb->setLayout(hlyout);
    hlyout2->addWidget(gb);

    menu = new QMenu;
    gb = new QGroupBox("");
    hlyout = new QHBoxLayout;
    pb = new QPushButton("Аварийная сигнализация");
    pb->setMinimumSize(QSize(230, 30));
    connect(pb, SIGNAL(clicked()), this, SIGNAL(ModuleAlarmButtonPressed()));
    hlyout->addWidget(pb, Qt::AlignRight);
    hlyout->addWidget(WDFunc::NewLBL(this, "", "", "952", pmgrn), 1);
    WDFunc::SetVisible(this, "952", false);
    gb->setLayout(hlyout);
    hlyout2->addWidget(gb);

    if (hlyout2->count())
        vlyout->addLayout(hlyout2);
    setLayout(vlyout);

    connect(Alarm, SIGNAL(SetFirstButton()), this, SLOT(UpdateFirstUSB()));
    connect(Alarm, SIGNAL(SetWarnAlarmColor(QList<bool>)), this, SLOT(UpdateSecondUSB(QList<bool>)));
    connect(Alarm, SIGNAL(SetAlarmColor(QList<bool>)), this, SLOT(UpdateThirdUSB(QList<bool>)));
}

void AlarmWidget::UpdateSecondUSB(QList<bool> warnalarm)
{
    // int i;
    int alarm = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmylw = new QPixmap("images/yellowc.png");

    // for(i=0; i<warnalarmcount.size(); i++)
    foreach (bool item, warnalarm)
    {
        if (item == true)
        {
            alarm++;
            break;
        }
    }

    WDFunc::SetLBLImage(this, "951", (alarm == 0) ? pmgrn : pmylw);
    WDFunc::SetVisible(this, "951", true);
}

void AlarmWidget::UpdateThirdUSB(QList<bool> avar)
{
    // int i;
    int alarm = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    //     for(i=0; i<alarmcount.size(); i++)
    foreach (bool item, avar)
    {
        if (item == true)
        {
            alarm++;
            break;
        }
    }

    WDFunc::SetLBLImage(this, "952", (alarm == 0) ? pmgrn : pmred);
    WDFunc::SetVisible(this, "952", true);
}

void AlarmWidget::UpdateFirstUSB()
{
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmylw = new QPixmap("images/yellowc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    if (ModuleBSI::ModuleBsi.Hth & WARNBSIMASK)
    {

        WDFunc::SetLBLImage(this, "950", pmylw);
        WDFunc::SetVisible(this, "950", true);
    }
    else if (ModuleBSI::ModuleBsi.Hth & AVARBSIMASK)
    {

        WDFunc::SetLBLImage(this, "950", pmred);
        WDFunc::SetVisible(this, "950", true);
    }
    else
    {

        WDFunc::SetLBLImage(this, "950", pmgrn);
        WDFunc::SetVisible(this, "950", true);
    }
}

void AlarmWidget::Clear()
{
    WDFunc::SetVisible(this, "950", false);
    WDFunc::SetVisible(this, "951", false);
    WDFunc::SetVisible(this, "952", false);
}
