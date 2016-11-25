#include <QCoreApplication>
#include <QtMath>
#include <QTime>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
//#include <QSpinBox>
#include "e_checkdialog.h"
#include "../publicclass.h"
#include "../canal.h"

e_checkdialog::e_checkdialog(QWidget *parent) :
    QDialog(parent)
{
    XlsxWriting = false;
    xlsx = 0;
    timer = new QTimer;
    timer->setObjectName("etunetimer");
    timer->setInterval(ANMEASINT);
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

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

    lyout = new QVBoxLayout;

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

    // CP2 - ДАННЫЕ МОДУЛЯ

    lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lbl = new QLabel("0.Температура:");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("value0");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,1);
    lbl = new QLabel("1.Частота:");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("value1");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,1);
    lyout->addLayout(hlyout);
    QGridLayout *glyout = new QGridLayout;
    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("IUNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+2));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+2)+".Истинные действующие значения сигналов");
        glyout->addWidget(lbl,1,i,1,1);
        lbl = new QLabel("IUF["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+8));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+8)+".Действующие значения сигналов по 1-й гармонике");
        glyout->addWidget(lbl,3,i,1,1);
        lbl = new QLabel("PHF["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+14));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+14)+".Угол сдвига между сигналами по первой гармонике");
        glyout->addWidget(lbl,5,i,1,1);
    }
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("PNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+20));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+20)+".Истинная активная мощность");
        glyout->addWidget(lbl,7,i,1,1);
        lbl = new QLabel("SNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+23));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+23)+".Кажущаяся полная мощность");
        glyout->addWidget(lbl,7,i+3,1,1);
        lbl = new QLabel("QNF["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+26));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+26)+".Реактивная мощность");
        glyout->addWidget(lbl,9,i,1,1);
        lbl = new QLabel("Cos["+QString::number(i)+"]");
        glyout->addWidget(lbl,8,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+29));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+29)+".Cos phi");
        glyout->addWidget(lbl,9,i+3,1,1);
        lbl = new QLabel("PF["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+32));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+32)+".Активная мощность по 1-й гармонике");
        glyout->addWidget(lbl,11,i,1,1);
        lbl = new QLabel("QF["+QString::number(i)+"]");
        glyout->addWidget(lbl,10,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+35));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+35)+".Реактивная мощность по 1-й гармонике");
        glyout->addWidget(lbl,11,i+3,1,1);
        lbl = new QLabel("SF["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+38));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+38)+".Полная мощность по 1-й гармонике");
        glyout->addWidget(lbl,13,i,1,1);
        lbl = new QLabel("CosPhi["+QString::number(i)+"]");
        glyout->addWidget(lbl,12,i+3,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+41));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+41)+".Cos phi по 1-й гармонике");
        glyout->addWidget(lbl,13,i+3,1,1);
    }
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("PHI["+QString::number(i)+"]");
        glyout->addWidget(lbl,14,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("value"+QString::number(i+44));
        lbl->setStyleSheet(ValuesFormat);
        lbl->setToolTip(QString::number(i+44)+".Угол между током и напряжением");
        glyout->addWidget(lbl,15,i,1,1);
    }
    lyout->addLayout(glyout);

    hlyout = new QHBoxLayout;
    lbl = new QLabel("Период обновления данных измерения, сек:");
    hlyout->addWidget(lbl);
    QRadioButton *rb = new QRadioButton;
    rb->setObjectName("500");
    rb->setText("0,5");
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("2000");
    rb->setText("2");
    rb->setChecked(true);
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("10000");
    rb->setText("10");
    connect(rb,SIGNAL(clicked()),this,SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    lyout->addLayout(hlyout);

    QPushButton *pb = new QPushButton("Запустить чтение аналоговых сигналов");
    pb->setObjectName("pbmeasurements");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartAnalogMeasurements()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Запустить чтение аналоговых сигналов в файл");
    pb->setObjectName("pbfilemeasurements");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartAnalogMeasurementsToFile()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Остановить чтение аналоговых сигналов");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopAnalogMeasurements()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);

    lyout->addStretch(1);
    cp2->setLayout(lyout);

    QGroupBox *gb = new QGroupBox("Проверки");
    gb1lyout = new QVBoxLayout;

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
    lyout->addWidget(CheckTW);
    setLayout(lyout);
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

void e_checkdialog::RefreshAnalogValues()
{
    QLabel *lbl;
    lbl = this->findChild<QLabel *>("value0");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bda_block.Tmk, 'f', 1));
    lbl = this->findChild<QLabel *>("value1");
    if (lbl == 0)
        return;
    lbl->setText(QString::number(Bda_block.Frequency, 'f', 4));
    for (int i = 0; i < 3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+2));
        if (lbl == 0)
            return;
/*        int Precision = (pc.ModuleBsi.MTypeB != MTE_2T0N) ? 3 : 4;
        lbl->setText(QString::number(Bda_block.IUefNat_filt[i], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+5));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_0T2N) ? 4 : 3;
        lbl->setText(QString::number(Bda_block.IUefNat_filt[i+3], 'f', Precision));

        lbl = this->findChild<QLabel *>("value"+QString::number(i+8));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_2T0N) ? 3 : 4;
        lbl->setText(QString::number(Bda_block.IUeff_filtered[i], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+11));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_0T2N) ? 4 : 3;
        lbl->setText(QString::number(Bda_block.IUeff_filtered[i+3], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+14));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.phi_next_f[i], 'f', 3));*/
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+20));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.PNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+23));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.SNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+26));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.QNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+29));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhiNat[i], 'f', 4));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+32));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Pf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+35));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Qf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+38));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Sf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+41));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhi[i], 'f', 4));
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+44));
        if (lbl == 0)
            return;
        float PHI = (180*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        lbl->setText(QString::number(PHI, 'f', 3));
    }
}

void e_checkdialog::StartAnalogMeasurementsToFile()
{
    QString Filename = QFileDialog::getSaveFileName(this,"Сохранить данные","","Excel files (*.xlsx)");
    if (Filename == "")
    {
        ECHECKER("Не задано имя файла");
        return; // !!! ошибка - не задано имя файла
    }
    XlsxWriting = true;
    xlsx = new QXlsx::Document(Filename);
    xlsx->write(1,1,QVariant("Модуль: "+pc.ModuleTypeString+" сер. ном. "+QString::number(pc.ModuleBsi.SerialNum,10)));
    xlsx->write(2,1,QVariant("Дата начала записи: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(3,1,QVariant("Время начала записи: "+QDateTime::currentDateTime().toString("hh:mm:ss")));
    xlsx->write(5,1,QVariant("Дата и время отсчёта"));
    for (int i=0; i<3; i++)
    {
/*        if (pc.ModuleBsi.MType1 != MTE_2T0N)
            xlsx->write(5,i+2,QVariant(("U1 ф")+QString::number(i+10, 36)+", В"));
        else
            xlsx->write(5,i+2,QVariant("I1 ф"+QString::number(i+10, 36)+", А"));

        if (pc.ModuleBsi.MType1 != MTE_0T2N)
            xlsx->write(5,i+5,QVariant("I2 ф"+QString::number(i+10, 36)+", А"));
        else
            xlsx->write(5,i+5,QVariant("U2 ф"+QString::number(i+10, 36)+", В"));

        xlsx->write(5,i+8,QVariant("Phi ф"+QString::number(i+10, 36)+", град"));
        xlsx->write(5,i+11,QVariant("Pf ф"+QString::number(i+10, 36)+", Вт"));
        xlsx->write(5,i+14,QVariant("Qf ф"+QString::number(i+10, 36)+", ВА"));
        xlsx->write(5,i+17,QVariant("Sf ф"+QString::number(i+10, 36)+", ВА"));*/
    }
    xlsx->write(5,20,QVariant("f, Гц"));
    xlsx->write(5,21,QVariant("t, град"));
    WRow = 6;
    QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
    if (pb != 0)
        pb->setEnabled(false);
    pb = this->findChild<QPushButton *>("pbmeasurements");
    if (pb != 0)
        pb->setEnabled(false);
    ElapsedTimeCounter = new QTime;
    ElapsedTimeCounter->start();
    StartAnalogMeasurements();
}

void e_checkdialog::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBdi, &Bda_block, sizeof(Bda_block));
    while (cn->Busy)
        qApp->processEvents();
    if (cn->result != CN_OK)
    {
        ECHECKINFO("Ошибка при приёме данных");
        return;
    }
    // обновление коэффициентов в соответствующих полях на экране
    RefreshAnalogValues();
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
        xlsx->write(WRow,1,QVariant(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz")));
        for (int i=0; i<3; i++)
        {
/*            QString Precision = (pc.ModuleBsi.MTypeB != MTE_2T0N) ? "0.000" : "0.0000";
            format.setNumberFormat(Precision);
            xlsx->write(WRow,i+2,Bda_block.IUeff_filtered[i],format);

            Precision = (pc.ModuleBsi.MTypeB != MTE_0T2N) ? "0.0000" : "0.000";
            format.setNumberFormat(Precision);
            xlsx->write(WRow,i+5,Bda_block.IUeff_filtered[i+3],format);

            format.setNumberFormat("0.000");
            float Phi = (static_cast<float>(180)*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
            xlsx->write(WRow,i+8,Phi,format);
            xlsx->write(WRow,i+11,Bda_block.Pf[i],format);
            xlsx->write(WRow,i+14,Bda_block.Qf[i],format);
            xlsx->write(WRow,i+17,Bda_block.Sf[i],format);*/
        }
        format.setNumberFormat("0.0000");
        xlsx->write(WRow,20,Bda_block.Frequency,format);
        format.setNumberFormat("0.0");
        xlsx->write(WRow,21,Bda_block.Tmk,format);
        WRow++;
    }
}

void e_checkdialog::StartAnalogMeasurements()
{
    timer->start();
}

void e_checkdialog::StopAnalogMeasurements()
{
    if (XlsxWriting)
    {
        if (xlsx)
        {
            xlsx->save();
            ECHECKINFO("Файл создан успешно");
            delete xlsx;
        }
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != 0)
        {
            pb->setEnabled(true);
            pb->setText("Запустить чтение аналоговых сигналов в файл");
        }
        pb = this->findChild<QPushButton *>("pbmeasurements");
        if (pb != 0)
            pb->setEnabled(true);
        XlsxWriting = false;
    }
    timer->stop();
}

void e_checkdialog::SetTimerPeriod()
{
    bool TimerIsActive = false;
    if (timer->isActive())
        TimerIsActive = true;
    bool ok;
    int per = sender()->objectName().toInt(&ok);
    if (!ok)
        return;
    timer->stop();
    timer->setInterval(per);
    if (TimerIsActive)
        timer->start();
}
