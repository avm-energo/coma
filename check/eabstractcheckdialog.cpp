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
#include "../gen/maindef.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../gen/colors.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

EAbstractCheckDialog::EAbstractCheckDialog(BoardTypes board, QWidget *parent) :
    QDialog(parent)
{
    XlsxWriting = false;
    Busy = false;
    xlsx = nullptr;
    WRow = 0;
    Board = board;
    Bd_blocks.clear();
    timer = new QTimer;
    timer->setObjectName("checktimer");
#if PROGSIZE != PROGSIZE_EMUL
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));
#endif
    timer->setInterval(1000);
    setAttribute(Qt::WA_DeleteOnClose);
}

void EAbstractCheckDialog::SetupUI(QStringList &tabnames)
{
    if (tabnames.size() < BdUINum)
    {
        ERMSG("Wrong BdTab size");
        return;
    }
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *CheckTW = new QTabWidget;
    QString ConfTWss = "QTabBar::tab {margin-right: 0px; margin-left: 0px; padding: 5px;}"
                       "QTabBar::tab:selected {background-color: "+QString(TABCOLORA1)+";"
                        "border: 1px solid #000000;"
                        "border-top-left-radius: 4px;"
                        "border-top-right-radius: 4px;"
                        "padding: 2px;"
                        "margin-left: -4px; margin-right: -4px;}" \
                       "QTabBar::tab:first:selected {margin-left: 0;}" \
                       "QTabBar::tab:last:selected {margin-right: 0;}" \
                       "QTabBar::tab:only-one {margin: 0;}";
    CheckTW->tabBar()->setStyleSheet(ConfTWss);
//    CheckTW->addTab(AutoCheckUI(),"  Автоматическая проверка  ");
    for (int i=0; i<BdUINum; ++i)
        CheckTW->addTab(BdUI(i),"  "+tabnames.at(i)+"  ");
    QWidget *w = CustomTab();
    if (w != nullptr)
        CheckTW->addTab(w, "  Прочее  ");
    lyout = new QVBoxLayout;
    lyout->addWidget(CheckTW);
    lyout->addWidget(BottomUI());
    setLayout(lyout);
}

void EAbstractCheckDialog::Check1PPS()
{

}

void EAbstractCheckDialog::SetBd(int bdnum, void *block, int blocksize, bool toxlsx)
{
    BdBlocks *bdblock = new BdBlocks;
    bdblock->block = block;
    bdblock->blocknum = blocksize;
    bdblock->toxlsxwrite = toxlsx;
    Bd_blocks[bdnum] = bdblock;
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

    QPushButton *pb = new QPushButton("Запустить чтение сигналов");
    pb->setObjectName("pbmeasurements");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(StartAnalogMeasurements()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Запустить чтение сигналов в файл");
    pb->setObjectName("pbfilemeasurements");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(StartAnalogMeasurementsToFile()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Остановить чтение сигналов");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(StopAnalogMeasurements()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    w->setLayout(lyout);
    return w;
}

/*void EAbstractCheckDialog::GetIP()
{
    if (Commands::GetIP(&Bip_block, sizeof(Bip)) != Error::NOERROR)
        EMessageBox::error(this, "Ошибка", "Ошибка получения данных по IP адресу модуля");
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

#if PROGSIZE != PROGSIZE_EMUL

void EAbstractCheckDialog::StartAnalogMeasurementsToFile()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString Filename = dlg->getSaveFileName(this, "Сохранить данные",StdFunc::GetHomeDir(),"Excel files (*.xlsx)", \
                                            Q_NULLPTR, QFileDialog::DontUseNativeDialog);
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
    xlsx->write(1,1,QVariant(tmps + ": "+ModuleBSI::GetModuleTypeString()+" сер. ном. " + \
                             QString::number(ModuleBSI::SerialNum(BoardTypes::BT_MODULE),10)));
    xlsx->write(2,1,QVariant("Дата начала записи: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(3,1,QVariant("Время начала записи: "+QDateTime::currentDateTime().toString("hh:mm:ss")));
    xlsx->write(5,1,QVariant("Дата и время отсчёта"));
    PrepareHeadersForFile(6); // в 6 ряду пишем заголовки
    WRow = 7;
    QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
    if (pb != nullptr)
        pb->setEnabled(false);
    pb = this->findChild<QPushButton *>("pbmeasurements");
    if (pb != nullptr)
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
        if (pb != nullptr)
        {
            int MSecs = ElapsedTimeCounter->elapsed();
            QString TimeElapsed = QTime::fromMSecsSinceStartOfDay(MSecs).toString("hh:mm:ss.zzz");
            pb->setText("Идёт запись: "+TimeElapsed);
        }
    }
    int bdkeyssize = Bd_blocks.keys().size();
    for (int bdnum = 0; bdnum < bdkeyssize; ++bdnum)
    {
        if (!XlsxWriting || (XlsxWriting && (Bd_blocks[Bd_blocks.keys().at(bdnum)]->toxlsxwrite)))
        {
            int tmpi = Bd_blocks.keys().at(bdnum);
            if (Commands::GetBd(tmpi, Bd_blocks[tmpi]->block, Bd_blocks[tmpi]->blocknum) != Error::ER_NOERROR)
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
//    Interval = ElapsedTimeCounter->elapsed() - Interval;
//    qDebug() << Interval;
    WRow++;
    Busy = false;
}

void EAbstractCheckDialog::StartAnalogMeasurements()
{
//    CurBdNum = 1;
    PrepareAnalogMeasurements();
    timer->start();
}

void EAbstractCheckDialog::StopAnalogMeasurements()
{
    if (XlsxWriting)
    {
        if (xlsx)
        {
            xlsx->save();
            EMessageBox::information(this, "Внимание", "Файл создан успешно");
            delete xlsx;
        }
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != nullptr)
        {
            pb->setEnabled(true);
            pb->setText("Запустить чтение аналоговых сигналов в файл");
        }
        pb = this->findChild<QPushButton *>("pbmeasurements");
        if (pb != nullptr)
            pb->setEnabled(true);
        XlsxWriting = false;
    }
    timer->stop();
}

void EAbstractCheckDialog::TimerTimeout()
{
    ReadAnalogMeasurementsAndWriteToFile();
}
#endif

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
