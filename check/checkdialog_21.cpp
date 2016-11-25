#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QCoreApplication>
#include <QFileDialog>
#include <QDateTime>
#include "a_checkdialog.h"
#include "../publicclass.h"
#include "../canal.h"

a_checkdialog::a_checkdialog(QWidget *parent) :
    QDialog(parent)
{
    BdMeasurementsActive = BdaMeasurementsActive = false;
    timer = new QTimer;
    timer->setObjectName("timer");
    timer->setInterval(1000); // per / 2
    OddTimeout = true;
    XlsxWriting = false;
    xlsx = 0;
    ElapsedTimeCounter = 0;
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));
    timer->start();
    setAttribute(Qt::WA_DeleteOnClose);
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
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
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
    QPushButton *pb = new QPushButton("Запустить измерения");
    pb->setObjectName("pbmeasurements");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMeasurements()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Запустить измерения с записью в файл");
    pb->setObjectName("pbfilemeasurements");
    if (pc.Emul)
        pb->setEnabled(false);
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMeasurementsWithFile()));
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Остановить измерения");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMeasurements()));
    gb1lyout->addWidget(pb);
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
    lbl = new QLabel("Период обновления данных измерения, сек:");
    gb2lyout->addWidget(lbl);
    QRadioButton *rb = new QRadioButton;
    rb->setObjectName("500");
    rb->setText("0,5");
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    gb2lyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("2000");
    rb->setText("2");
    rb->setChecked(true);
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    gb2lyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("10000");
    rb->setText("10");
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    gb2lyout->addWidget(rb);
    gb->setLayout(gb2lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Проверки");
    gb1lyout = new QVBoxLayout;
 /*   gb2lyout = new QHBoxLayout;
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
    gb1lyout->addLayout(gb2lyout); */

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

void a_checkdialog::StartMeasurementsWithFile()
{
    aconf = new config_21;
    // читаем текущую конфигурацию
    cn->Send(CN_GF,NULL,0,1,aconf->Config);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result != CN_OK)
    {
        ACHECKER("Ошибка приёма данных конфигурации");
        return;
    }
    QString Filename = QFileDialog::getSaveFileName(this,"Сохранить данные","","Excel files (*.xlsx)");
    if (Filename == "")
    {
        ACHECKER("Не задано имя файла");
        return; // !!! ошибка - не задано имя файла
    }
    XlsxWriting = true;
    xlsx = new QXlsx::Document(Filename);
    xlsx->write(1,1,QVariant("Модуль: "+pc.ModuleTypeString+" сер. ном. "+QString::number(pc.ModuleBsi.SerialNum,10)));
    xlsx->write(2,1,QVariant("Дата начала записи: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(3,1,QVariant("Время начала записи: "+QDateTime::currentDateTime().toString("hh:mm:ss")));
    xlsx->write(5,1,QVariant("Дата и время отсчёта"));
    for (int i=0; i<16; i++)
    {
        if (aconf->Bci_block.in_type[i] == INTYPEMA)
            xlsx->write(5,i+3,"I"+QString::number(i)+", мА");
        else if (aconf->Bci_block.in_type[i] == INTYPEV)
            xlsx->write(5,i+3,"U"+QString::number(i)+", В");
        else
            continue;
    }
    xlsx->write(5,2,QVariant("t, град"));
    WRow = 6;
    QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
    if (pb != 0)
        pb->setEnabled(false);
    pb = this->findChild<QPushButton *>("pbmeasurements");
    if (pb != 0)
        pb->setEnabled(false);
    ElapsedTimeCounter = new QTime;
    ElapsedTimeCounter->start();
    StartMeasurements();
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
    if (XlsxWriting)
    {
        if (xlsx)
        {
            xlsx->save();
            ACHECKINFO("Файл создан успешно");
            delete xlsx;
        }
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != 0)
        {
            pb->setEnabled(true);
            pb->setText("Запустить измерения с записью в файл");
        }
        pb = this->findChild<QPushButton *>("pbmeasurements");
        if (pb != 0)
            pb->setEnabled(true);
        XlsxWriting = false;
    }
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

void a_checkdialog::SetTimerPeriod()
{
    bool TimerIsActive = false;
    if (timer->isActive())
        TimerIsActive = true;
    bool ok;
    int per = sender()->objectName().toInt(&ok);
    if (!ok)
        return;
    timer->stop();
    timer->setInterval(per / 2); // /2, т.к. измерения проводятся за два раза - Bd и Bda
    if (TimerIsActive)
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
        if (XlsxWriting)
        {
            QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
            if (pb != 0)
            {
                int MSecs = ElapsedTimeCounter->elapsed();
                QString TimeElapsed = QTime::fromMSecsSinceStartOfDay(MSecs).toString("hh:mm:ss.zzz");
                pb->setText("Идёт запись: "+TimeElapsed);
            }
            QXlsx::Format format;
            format.setNumberFormat("0.0000");
            xlsx->write(WRow,1,QVariant(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz")));
            for (int i=0; i<16; i++)
            {
                if (aconf->Bci_block.in_type[i] == INTYPEMA)
                    xlsx->write(WRow,i+3,Bd_block.inI[i],format);
                else if (aconf->Bci_block.in_type[i] == INTYPEV)
                    xlsx->write(WRow,i+3,Bd_block.inU[i],format);
                else
                    continue;
            }
            xlsx->write(WRow,2,Bd_block.at,format);
            WRow++;

        }
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
