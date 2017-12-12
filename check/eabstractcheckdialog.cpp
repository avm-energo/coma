#include <QCoreApplication>
#include <QtTest/QTest>
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
#include "eabstractcheckdialog.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/publicclass.h"
#include "../gen/commands.h"

EAbstractCheckDialog::EAbstractCheckDialog(QWidget *parent) :
    QDialog(parent)
{
//    Parent = parent;
    XlsxWriting = false;
    Busy = false;
    xlsx = 0;
//    CurBdNum = 1;
    WRow = 0;
    Bd_blocks.clear();
    timer = new QTimer;
    timer->setObjectName("checktimer");
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));
    setAttribute(Qt::WA_DeleteOnClose);
}

void EAbstractCheckDialog::Check1PPS()
{

}

void EAbstractCheckDialog::SetBd(int bdnum, void *block, int blocksize, bool toxlsx)
{
    BdBlocks *bdblock = new BdBlocks;
    while (bdnum >= Bd_blocks.size())
        Bd_blocks.append(new BdBlocks);
    bdblock->block = block;
    bdblock->blocknum = blocksize;
    bdblock->toxlsxwrite = toxlsx;
    Bd_blocks.replace(bdnum, bdblock);
}

QWidget *EAbstractCheckDialog::BottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Период обновления данных измерения, сек:");
    hlyout->addWidget(lbl);
    QRadioButton *rb = new QRadioButton;
    rb->setObjectName("1000");
    rb->setText("1");
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
    w->setLayout(lyout);
    return w;
}

/*void EAbstractCheckDialog::GetIP()
{
    if (Commands::GetIP(&Bip_block, sizeof(Bip)) != NOERROR)
        MessageBox2::error(this, "Ошибка", "Ошибка получения данных по IP адресу модуля");
    else
        CheckIP();
}

void EAbstractCheckDialog::CheckIP()
{
    QLabel *lbl = this->findChild<QLabel *>("ipl");
    if (lbl == 0)
        return;
    for (int i = 0; i < 4; i++)
        lbl->text().append(QString::number(Bip_block.ip[i], 16) + ".");
    lbl->text().chop(1);
} */

void EAbstractCheckDialog::StartAnalogMeasurementsToFile()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString Filename = dlg->getSaveFileName(this, "Сохранить данные",pc.HomeDir,"Excel files (*.xlsx)", Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    dlg->close();
    if (Filename == "")
    {
        ERMSG("Не задано имя файла");
        return; // !!! ошибка - не задано имя файла
    }
    if (Filename.split(".").last() != "xlsx")
        Filename += ".xlsx";
    // удаляем файл, если он есть
    QFile fn;
    fn.setFileName(Filename);
    if (fn.exists())
        fn.remove();
    XlsxWriting = true;
    xlsx = new QXlsx::Document(Filename);
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "Модуль" : "Прибор");
    xlsx->write(1,1,QVariant(tmps + ": "+pc.ModuleTypeString+" сер. ном. "+QString::number(pc.ModuleBsi.SerialNum,10)));
    xlsx->write(2,1,QVariant("Дата начала записи: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(3,1,QVariant("Время начала записи: "+QDateTime::currentDateTime().toString("hh:mm:ss")));
    xlsx->write(5,1,QVariant("Дата и время отсчёта"));
    PrepareHeadersForFile(6); // в 6 ряду пишем заголовки
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

void EAbstractCheckDialog::ReadAnalogMeasurementsAndWriteToFile()
{
//    int Interval = ElapsedTimeCounter->elapsed();
//    qDebug() << ElapsedTimeCounter->elapsed();
    // получение текущих аналоговых сигналов от модуля
    if (Busy)
        return;
    Busy = true;
    if (XlsxWriting)
    {
        xlsx->write(WRow,1,QVariant(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != 0)
        {
            int MSecs = ElapsedTimeCounter->elapsed();
            QString TimeElapsed = QTime::fromMSecsSinceStartOfDay(MSecs).toString("hh:mm:ss.zzz");
            pb->setText("Идёт запись: "+TimeElapsed);
        }
    }
    for (int bdnum = 0; bdnum <= BdNum; ++bdnum)
    {
        if (bdnum < Bd_blocks.size())
        {
            if (!XlsxWriting || (XlsxWriting && (Bd_blocks.at(bdnum)->toxlsxwrite)))
            {
                if (Commands::GetBd(bdnum, Bd_blocks.at(bdnum)->block, Bd_blocks.at(bdnum)->blocknum) != NOERROR)
                {
                    WARNMSG("Ошибка при приёме данных");
                    Busy = false;
                    return;
                }
                // обновление коэффициентов в соответствующих полях на экране
                RefreshAnalogValues(bdnum);
                if (XlsxWriting)
                    WriteToFile(WRow, bdnum);
            }
        }
        else
        {
            WARNMSG("Передан некорректный номер блока");
            Busy = false;
            return;
        }
    }
//    Interval = ElapsedTimeCounter->elapsed() - Interval;
//    qDebug() << Interval;
    WRow++;
    Busy = false;
}

void EAbstractCheckDialog::StartAnalogMeasurements()
{
//    CurBdNum = 1;
    timer->start();
}

void EAbstractCheckDialog::StopAnalogMeasurements()
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

void EAbstractCheckDialog::SetTimerPeriod()
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

void EAbstractCheckDialog::TimerTimeout()
{
    ReadAnalogMeasurementsAndWriteToFile();
}
