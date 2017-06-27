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
#include "abstractcheckdialog.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../publicclass.h"
#include "../canal.h"

AbstractCheckDialog::AbstractCheckDialog(QWidget *parent) :
    QDialog(parent)
{
    XlsxWriting = false;
    xlsx = 0;
    timer = new QTimer;
    timer->setObjectName("checktimer");
    timer->setInterval(ANMEASINT);
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void AbstractCheckDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *CheckTW = new QTabWidget;
    CheckTW->addTab(AutoCheckUI(),"Автоматическая проверка");
    CheckTW->addTab(Bd1UI(),"Текущие измерения гр. 1");
    CheckTW->addTab(Bd2UI(),"Текущие измерения гр. 2");
    CheckTW->addTab(Bd3UI(),"Текущие измерения гр. 3");
    lyout->addWidget(CheckTW);

    lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
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
    setLayout(lyout);
}

void AbstractCheckDialog::Check1PPS()
{

}

void AbstractCheckDialog::GetIP()
{
    connect(cn,SIGNAL(sendend()),this,SLOT(CheckIP()));
    cn->Send(CN_IP, Canal::BT_NONE, &Bip_block, sizeof(Bip));
}

void AbstractCheckDialog::CheckIP()
{
    disconnect(cn,SIGNAL(sendend()),this,SLOT(CheckIP()));
    QLabel *lbl = this->findChild<QLabel *>("ipl");
    if (lbl == 0)
        return;
    for (int i = 0; i < 4; i++)
        lbl->text().append(QString::number(Bip_block.ip[i], 16) + ".");
    lbl->text().chop(1);
}

void AbstractCheckDialog::RefreshAnalogValues()
{
    QLabel *lbl;
    WDFunc::SetLBLText(this, "value0", QString::number(Bda_block.Tmk, 'f', 1));
    WDFunc::SetLBLText(this, "value1", QString::number(Bda_block.Vbat, 'f', 1));
    WDFunc::SetLBLText(this, "value2", QString::number(Bda_block.Frequency, 'f', 1));
    for (int i = 0; i < 3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+3));
        if (lbl == 0)
            return;
/*        int Precision = (pc.ModuleBsi.MTypeB != MTE_2T0N) ? 3 : 4;
        lbl->setText(QString::number(Bda_block.IUefNat_filt[i], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+6));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_0T2N) ? 4 : 3;
        lbl->setText(QString::number(Bda_block.IUefNat_filt[i+3], 'f', Precision));

        lbl = this->findChild<QLabel *>("value"+QString::number(i+9));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_2T0N) ? 3 : 4;
        lbl->setText(QString::number(Bda_block.IUeff_filtered[i], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+12));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_0T2N) ? 4 : 3;
        lbl->setText(QString::number(Bda_block.IUeff_filtered[i+3], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+15));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.phi_next_f[i], 'f', 3));*/
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+21));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.PNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+24));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.SNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+27));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.QNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+30));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhiNat[i], 'f', 4));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+33));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Pf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+36));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Qf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+39));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Sf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+42));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhi[i], 'f', 4));
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+45));
        if (lbl == 0)
            return;
        float PHI = (180*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        lbl->setText(QString::number(PHI, 'f', 3));
    }
}

void AbstractCheckDialog::StartAnalogMeasurementsToFile()
{
    QString Filename = QFileDialog::getSaveFileName(this,"Сохранить данные","","Excel files (*.xlsx)");
    if (Filename == "")
    {
        ERMSG("Не задано имя файла");
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

void AbstractCheckDialog::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBd, Canal::BT_NONE, &Bda_block, sizeof(Bda_block));
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме данных");
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

void AbstractCheckDialog::StartAnalogMeasurements()
{
    timer->start();
}

void AbstractCheckDialog::StopAnalogMeasurements()
{
    if (XlsxWriting)
    {
        if (xlsx)
        {
            xlsx->save();
            MessageBox2::information(this, "Внимание", "Файл создан успешно");
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

void AbstractCheckDialog::SetTimerPeriod()
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
