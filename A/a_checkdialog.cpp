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
#include <QCoreApplication>
#include "a_checkdialog.h"
#include "../publicclass.h"
#include "../canal.h"

a_checkdialog::a_checkdialog(QWidget *parent) :
    QDialog(parent)
{
    BdMeasurementsActive = BdaMeasurementsActive = false;
    timer = new QTimer;
    timer->setObjectName("timer");
    timer->setInterval(1000);
    OddTimeout = true;
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));
    timer->start();
    setAttribute(Qt::WA_DeleteOnClose);
//    cn = new canal;
    for (int i = 0; i < 16; i++)
    {
        Bda_block.sin[i] = 0;
        Bd_block.inI[i] = 0;
        Bd_block.inU[i] = 0;
        Bd_block.ain[i] = 0;
        Bd_block.ainn[i] = 0;
        Bd_block.ainx[i] = 0;
    }
    Bd_block.at = 0;
    SetupUI();
}

void a_checkdialog::SetupUI()
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

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Тек. измерения, приведённые ко входу (ток)");
    gb2lyout->addWidget(lbl,0);
    gb2lyout->addWidget(line,1);
    gb1lyout->addLayout(gb2lyout);
    gb2lyout = new QHBoxLayout;
//    QGroupBox *gb = new QGroupBox("Тек. измерения, приведённые ко входу");
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("bdinI"+QString::number(i));
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
/*    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Тек. измерения, приведённые ко входу (напряжения)");
    gb1lyout = new QVBoxLayout; */
    gb2lyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Тек. измерения, приведённые ко входу (напряжения)");
    gb2lyout->addWidget(lbl,0);
    gb2lyout->addWidget(line,1);
    gb1lyout->addLayout(gb2lyout);
    gb2lyout = new QHBoxLayout;
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("bdinU"+QString::number(i));
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
/*    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Тек. измерения в инж. единицах");
    gb1lyout = new QVBoxLayout;*/
    gb2lyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Тек. измерения в инженерных единицах");
    gb2lyout->addWidget(lbl,0);
    gb2lyout->addWidget(line,1);
    gb1lyout->addLayout(gb2lyout);
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
/*    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Текущие минимумы по каналам");
    gb1lyout = new QVBoxLayout; */
    gb2lyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Текущие минимумы по каналам");
    gb2lyout->addWidget(lbl,0);
    gb2lyout->addWidget(line,1);
    gb1lyout->addLayout(gb2lyout);
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
/*    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Текущие максимумы по каналам");
    gb1lyout = new QVBoxLayout; */
    gb2lyout = new QHBoxLayout;
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lbl = new QLabel("Текущие максимумы по каналам");
    gb2lyout->addWidget(lbl,0);
    gb2lyout->addWidget(line,1);
    gb1lyout->addLayout(gb2lyout);
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
/*    gb->setLayout(gb1lyout);
    lyout->addWidget(gb); */
    lyout->addLayout(gb1lyout);
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
    if (pc.Emul)
        pb->setEnabled(false);
    gb2lyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMeasurements()));
    gb2lyout->addWidget(pb);
    gb1lyout->addLayout(gb2lyout);
    lyout->addLayout(gb1lyout);
    cp2->setLayout(lyout);

    lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Данные без подстройки");
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
    if (pc.Emul)
        pb->setEnabled(false);
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
    if (pc.Emul)
        pb->setEnabled(false);
    gb2lyout->addWidget(pb);
    pb = new QPushButton("Выключить светодиод");
    connect(pb,SIGNAL(clicked()),this,SLOT(CheckLEDOff()));
    if (pc.Emul)
        pb->setEnabled(false);
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
    if (pc.Emul)
        pb->setEnabled(false);
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
    if (pc.Emul)
        pb->setEnabled(false);
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

void a_checkdialog::StartMeasurements()
{
    BdMeasurementsActive = true;
}

void a_checkdialog::StartBdaMeasurements()
{
    BdaMeasurementsActive = true;
}

void a_checkdialog::StopMeasurements()
{
    BdMeasurementsActive = false;
}

void a_checkdialog::StopBdaMeasurements()
{
    BdaMeasurementsActive = false;
}

void a_checkdialog::RefreshBd()
{
    int i;
    for (i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("bdinI"+QString::number(i));
        if (lbl != 0)
            lbl->setText(QString::number(Bd_block.inI[i],'f',5));
        lbl = this->findChild<QLabel *>("bdinU"+QString::number(i));
        if (lbl != 0)
            lbl->setText(QString::number(Bd_block.inU[i],'f',5));
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

void a_checkdialog::RefreshBda()
{
    int i;
    for (i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("bda"+QString::number(i));
        if (lbl != 0)
            lbl->setText(QString::number(Bda_block.sin[i]/ACHECKNUMPOINTS,16));
    }
}

void a_checkdialog::SetTimerPeriod(int per)
{
    timer->stop();
    timer->setInterval(per / 2); // /2, т.к. измерения проводятся за два раза - Bd и Bda
    timer->start();
}

void a_checkdialog::CheckLEDOn()
{
    cn->Send(CN_Cln);
}

void a_checkdialog::CheckLEDOff()
{
    cn->Send(CN_Clf);
}

void a_checkdialog::Check1PPS()
{

}

void a_checkdialog::GetIP()
{
    cn->Send(CN_Gip, &Bip_block, sizeof(Bip));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckIP();
}

void a_checkdialog::CheckIP()
{
    QLabel *lbl = this->findChild<QLabel *>("ipl");
    if (lbl == 0)
        return;
    for (int i = 0; i < 4; i++)
        lbl->text().append(QString::number(Bip_block.ip[i], 16) + ".");
    lbl->text().chop(1);
}

void a_checkdialog::TimerTimeout()
{
    if ((BdMeasurementsActive) && (OddTimeout)) // текущие измерения проводим на первом проходе таймера
    {
        cn->Send(CN_GBd, &Bd_block, sizeof(Bd));
        while (cn->Busy)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (cn->result == CN_OK)
            RefreshBd();
    }
    if ((BdaMeasurementsActive) && (!OddTimeout)) // все остальные - на втором
    {
        cn->Send(CN_Gda, &Bda_block, sizeof(Bda));
        while (cn->Busy)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (cn->result == CN_OK)
            RefreshBda();
    }
    OddTimeout = !OddTimeout;
}
