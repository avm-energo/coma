#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include "e_checkdialog.h"
#include "../config.h"
#include "../publicclass.h"
#include "../canal.h"

e_checkdialog::e_checkdialog(QWidget *parent) :
    QDialog(parent)
{
/*    BdMeasurementsActive = BdaMeasurementsActive = false;
    timer = new QTimer;
    timer->setObjectName("timer");
    timer->setInterval(2000);
    OddTimeout = true;
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));
    timer->start();
    setAttribute(Qt::WA_DeleteOnClose);
//    cn = new canal;
    for (int i = 0; i < 16; i++)
    {
        Bda_block.sin[i] = 0;
        Bd_block.in[i] = 0;
        Bd_block.ain[i] = 0;
        Bd_block.ainn[i] = 0;
        Bd_block.ainx[i] = 0;
    }
    Bd_block.at = 0;
    SetupUI(); */
}
/*
void e_checkdialog::SetupUI()
{
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QVBoxLayout *gb1lyout = new QVBoxLayout;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QTabWidget *CheckTW = new QTabWidget;
    CheckTW->addTab(cp1,"Автоматическая проверка");
    CheckTW->addTab(cp2,"Текущие измерения");
    CheckTW->addTab(cp3,"Прочее");

    QGroupBox *gb = new QGroupBox("Тек. измерения, приведённые ко входу");
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("bdin"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        if ((i>0)&&!((i+1)%6))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Тек. измерения в инж. единицах");
    gb1lyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("bdain"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        if ((i>0)&&!((i+1)%6))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Текущие минимумы по каналам");
    gb1lyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("bdainn"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        if ((i>0)&&!((i+1)%6))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Текущие максимумы по каналам");
    gb1lyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("bdainx"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        if ((i>0)&&!((i+1)%6))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    lbl = new QLabel("Температура в кристалле:");
    gb2lyout = new QHBoxLayout;
    gb1lyout = new QVBoxLayout;
    gb2lyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setStyleSheet(ValuesFormat);
    lbl->setObjectName("bdat");
    gb2lyout->addWidget(lbl);
    gb1lyout->addLayout(gb2lyout);
    gb2lyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Запустить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMeasurements()));
    gb2lyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMeasurements()));
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);
    lyout->addLayout(gb1lyout);
    cp2->setLayout(lyout);

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Данные без подстройки");
    gb1lyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("bda"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 5);
        if ((i>1)&&!((i+1)%6))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);

    gb2lyout = new QHBoxLayout;
    pb = new QPushButton("Запустить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartBdaMeasurements()));
    gb2lyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopBdaMeasurements()));
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройки");
    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Период обновления данных измерения:");
    gb2lyout->addWidget(lbl);
    QSpinBox *spb = new QSpinBox;
    spb->setMinimum(500);
    spb->setMaximum(5000);
    spb->setSingleStep(250);
    spb->setValue(2000);
    connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetTimerPeriod(int)));
    gb2lyout->addWidget(spb);
    gb->setLayout(gb2lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Проверки");
    gb1lyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Проверка программно управляемого светодиода:");
    gb2lyout->addWidget(lbl);
    pb = new QPushButton("Включить светодиод");
    connect(pb,SIGNAL(clicked()),this,SLOT(CheckLEDOn()));
    gb2lyout->addWidget(pb);
    pb = new QPushButton("Выключить светодиод");
    connect(pb,SIGNAL(clicked()),this,SLOT(CheckLEDOff()));
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);

    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Текущий IP-адрес:");
    gb2lyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("ipl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    pb = new QPushButton("Обновить");
    connect(pb,SIGNAL(clicked()),this,SLOT(GetIP()));
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);

    gb2lyout = new QHBoxLayout;
    lbl = new QLabel("Контроль 1PPS:");
    gb2lyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("ppsl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    pb = new QPushButton("Обновить");
    connect(pb,SIGNAL(clicked()),this,SLOT(Check1PPS()));
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);

    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);
    cp3->setLayout(lyout);

    lyout = new QVBoxLayout;

    gb2lyout=new QHBoxLayout;
    lbl=new QLabel("Проверка светодиодного индикатора...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("aledl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    gb2lyout = new QHBoxLayout;
    lbl=new QLabel("Проверка каналов измерения...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("ainl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    gb2lyout=new QHBoxLayout;
    lbl=new QLabel("Проверка настроечных параметров...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("asetl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    gb2lyout=new QHBoxLayout;
    lbl=new QLabel("Проверка контактов IP...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("aipl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    gb2lyout=new QHBoxLayout;
    lbl=new QLabel("Проверка приёма сигнала 1PPS...");
    gb2lyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("appsl");
    lbl->setStyleSheet(ValuesFormat);
    gb2lyout->addWidget(lbl);
    lyout->addLayout(gb2lyout);
    lyout->addStretch(99);
    cp1->setLayout(lyout);
    lyout = new QVBoxLayout;
    lyout->addWidget(CheckTW);
    setLayout(lyout);
}

void e_checkdialog::StartMeasurements()
{
    BdMeasurementsActive = true;
}

void e_checkdialog::StartBdaMeasurements()
{
    BdaMeasurementsActive = true;
}

void e_checkdialog::StopMeasurements()
{
    BdMeasurementsActive = false;
}

void e_checkdialog::StopBdaMeasurements()
{
    BdaMeasurementsActive = false;
}

void e_checkdialog::RefreshBd()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(RefreshBd()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    int i;
    for (i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("bdin"+QString::number(i));
        if (lbl != 0)
            lbl->setText(QString::number(Bd_block.in[i],'f',5));
        lbl = this->findChild<QLabel *>("bdain"+QString::number(i));
        if (lbl != 0)
            lbl->setText(QString::number(Bd_block.ain[i],'f',5));
        lbl = this->findChild<QLabel *>("bdainn"+QString::number(i));
        if (lbl != 0)
            lbl->setText(QString::number(Bd_block.ainn[i],'f',5));
        lbl = this->findChild<QLabel *>("bdainx"+QString::number(i));
        if (lbl != 0)
            lbl->setText(QString::number(Bd_block.ainx[i],'f',5));
    }
    QLabel *lbl = this->findChild<QLabel *>("bdat");
    if (lbl != 0)
        lbl->setText(QString::number(Bd_block.at,'f',5));
}

void e_checkdialog::RefreshBda()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(RefreshBda()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    int i;
    for (i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("bda"+QString::number(i));
        if (lbl != 0)
            lbl->setText(QString::number(Bda_block.sin[i]/Z,16));
    }
}

void e_checkdialog::SetTimerPeriod(int per)
{
    timer->stop();
    timer->setInterval(per);
    timer->start();
}

void e_checkdialog::CheckLEDOn()
{
    cn->Send(CN_Cln);
}

void e_checkdialog::CheckLEDOff()
{
    cn->Send(CN_Clf);
}

void e_checkdialog::Check1PPS()
{

}

void e_checkdialog::GetIP()
{
    connect(cn,SIGNAL(DataReady()),this,SLOT(CheckIP()));
    cn->Send(CN_Gip, &Bip_block, sizeof(Bip));
}

void e_checkdialog::CheckIP()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(CheckIP()));
    QLabel *lbl = this->findChild<QLabel *>("ipl");
    if (lbl == 0)
        return;
    for (int i = 0; i < 4; i++)
        lbl->text().append(QString::number(Bip_block.ip[i], 16) + ".");
    lbl->text().chop(1);
}

void e_checkdialog::TimerTimeout()
{
    if ((BdMeasurementsActive) && (OddTimeout)) // текущие измерения проводим на первом проходе таймера
    {
        connect(cn,SIGNAL(DataReady()),this,SLOT(RefreshBd()));
        cn->Send(CN_GBd, &Bd_block, sizeof(Bd));
    }
    if ((BdaMeasurementsActive) && (!OddTimeout)) // все остальные - на втором
    {
        connect(cn,SIGNAL(DataReady()),this,SLOT(RefreshBda()));
        cn->Send(CN_Gda, &Bda_block, sizeof(Bda));
    }
    OddTimeout = !OddTimeout;
}

void e_checkdialog::ShowErrMsg(int ermsg)
{
    QMessageBox::critical(this,"error!",pc.errmsgs.at(ermsg));
}
*/
