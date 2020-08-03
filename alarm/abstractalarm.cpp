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

AbstractAlarm::AbstractAlarm(QWidget *parent):
      QDialog(parent)
{

   //  ReleWidget();

}

QWidget *AbstractAlarm::AlmReleWidget()
{
QMenu *menu = new QMenu;
QString tmps = "QMenuBar {background-color: "+QString(MAINWINCLR)+";}"\
        "QMenuBar::item {background-color: "+QString(MAINWINCLR)+";}";
menu->setStyleSheet(tmps);
QVBoxLayout *vlyout = new QVBoxLayout;
QHBoxLayout *hlyout1 = new QHBoxLayout;
QHBoxLayout *hlyout2 = new QHBoxLayout;
QWidget *w = new QWidget;
QStringList Discription =  QStringList() << "Состояние устройства" << "Предупредительная сигнализация" << "Аварийная сигнализация";
w->setStyleSheet("QComa {background-color: "+QString(MAINWINCLR)+";}");
QPixmap *pmgrn = new QPixmap("images/greenc.png");
QPushButton *pb = new QPushButton("Состояние устройства");
pb->setMinimumSize(QSize(230,30));
connect(pb,SIGNAL(clicked()),this,SLOT(DeviceState()));
QGroupBox *gb = new QGroupBox("");
hlyout1->addWidget(pb,Qt::AlignRight);
hlyout1->addWidget(WDFunc::NewLBL(w, "", "", "950", pmgrn), 1);
gb->setLayout(hlyout1);
hlyout2->addWidget(gb);


gb = new QGroupBox("");
hlyout1 = new QHBoxLayout;
pb = new QPushButton("Предупредительная сигнализация");
pb->setMinimumSize(QSize(230,30));
connect(pb,SIGNAL(clicked()),this,SLOT(PredAlarmState()));
hlyout1->addWidget(pb,Qt::AlignRight);
hlyout1->addWidget(WDFunc::NewLBL(w, "", "", "951", pmgrn), 1);
gb->setLayout(hlyout1);
hlyout2->addWidget(gb);

menu = new QMenu;
gb = new QGroupBox("");
hlyout1 = new QHBoxLayout;
pb = new QPushButton("Аварийная сигнализация");
pb->setMinimumSize(QSize(230,30));
connect(pb,SIGNAL(clicked()),this,SLOT(AlarmState()));
hlyout1->addWidget(pb,Qt::AlignRight);
hlyout1->addWidget(WDFunc::NewLBL(w, "", "", "952", pmgrn), 1);
gb->setLayout(hlyout1);
hlyout2->addWidget(gb);

if (hlyout2->count())
vlyout->addLayout(hlyout2);
w->setLayout(vlyout);
return w;

}
