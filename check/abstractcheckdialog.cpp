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
    CurBdNum = 0;
    WRow = 0;
    Bd_blocks.clear();
    timer = new QTimer;
    timer->setObjectName("checktimer");
    timer->setInterval(ANMEASINT);
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));
    setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractCheckDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *CheckTW = new QTabWidget;
    CheckTW->addTab(AutoCheckUI(),"Автоматическая проверка");
    for (int i=0; i<BdNum; ++i)
        CheckTW->addTab(BdUI(i),"Текущие измерения гр. "+QString::number(i));
    lyout->addWidget(CheckTW);

    lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Период обновления данных измерения, сек:");
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
    cn->Send(CN_IP, Canal::BT_NONE, &Bip_block, sizeof(Bip));
    if (cn->result != NOERROR)
        MessageBox2::error(this, "Ошибка", "Ошибка получения данных по IP адресу модуля");
    else
        CheckIP();
}

void AbstractCheckDialog::CheckIP()
{
    QLabel *lbl = this->findChild<QLabel *>("ipl");
    if (lbl == 0)
        return;
    for (int i = 0; i < 4; i++)
        lbl->text().append(QString::number(Bip_block.ip[i], 16) + ".");
    lbl->text().chop(1);
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
    PrepareHeadersForFile(6);
    WRow = 7;
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

void AbstractCheckDialog::ReadAnalogMeasurementsAndWriteToFile(int bdnum)
{
    // получение текущих аналоговых сигналов от модуля
    if (bdnum < Bd_blocks.size())
    {
        cn->Send(CN_GBd, bdnum, Bd_blocks.at(bdnum).block, Bd_blocks.at(bdnum).blocknum);
        if (cn->result != NOERROR)
        {
            WARNMSG("Ошибка при приёме данных");
            return;
        }
        // обновление коэффициентов в соответствующих полях на экране
        RefreshAnalogValues(bdnum);
        if (XlsxWriting)
        {
            QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
            if (pb != 0)
            {
                int MSecs = ElapsedTimeCounter->elapsed();
                QString TimeElapsed = QTime::fromMSecsSinceStartOfDay(MSecs).toString("hh:mm:ss.zzz");
                pb->setText("Идёт запись: "+TimeElapsed);
            }
            xlsx->write(WRow,1,QVariant(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz")));
            WriteToFile(WRow, bdnum);
            WRow++;
        }
    }
    else
        WARNMSG("Передан некорректный номер блока");
}

void AbstractCheckDialog::StartAnalogMeasurements()
{
    CurBdNum = 0;
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

void AbstractCheckDialog::TimerTimeout()
{
    ReadAnalogMeasurementsAndWriteToFile(CurBdNum);
    ++CurBdNum;
}
