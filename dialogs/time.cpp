#include <QGroupBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QTime>
#include <QTimer>
#include <QThread>
#include "../widgets/emessagebox.h"
#include "../widgets/ecombobox.h"
#include "../widgets/wd_func.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../gen/commands.h"
#include "../gen/error.h"
#include "../dialogs/time.h"
#include "../gen/timefunc.h"
#include "../gen/mainwindow.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

MNKTime::MNKTime(QWidget *parent) :
    QDialog(parent)
{
    //int i;

    FinishThread = true;
    closeThr = false;
    haveFinished = false;
    myDateTime.setTime_t(unixtimestamp);
    //setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    //thr->start();
}

MNKTime::~MNKTime()
{
   //deleteLater();
}

void MNKTime::SetupUI()
{
    QString phase[3] = {"Фаза A:","Фаза B:","Фаза C:"};
    //QString S;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *extraconf = new QWidget;
    QWidget *MEKconf = new QWidget;
    QWidget *time = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    analog1->setStyleSheet(tmps);
    analog2->setStyleSheet(tmps);
    extraconf->setStyleSheet(tmps);
    time->setStyleSheet(tmps);
    MEKconf->setStyleSheet(tmps);
    int row = 0;


    QString paramcolor = MAINWINCLR;

    QGroupBox *gb = new QGroupBox;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    SysTime = new QLabel;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slot_timeOut()));

    //SysTime->setText(QTime::currentTime().toString("hh:mm:ss"));

    timer->start(1000);

    QString Text = "dd-MM-yyyy HH:mm:ss";

    QDateTime current = QDateTime::currentDateTime();
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время ПК:"), row,1,1,1, Qt::AlignTop);
    QDateTime dt = QDateTime::currentDateTime();
    //SysTime->setText(dt.toString());
    SysTime->setText(QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ss"));
    glyout->addWidget(SysTime, row,2,1,4, Qt::AlignTop);

    row++;
    tmps = "QWidget {background-color: "+QString(MAINWINCLR)+";}";
    QPushButton *Button = new QPushButton("Записать дату и время ПК в модуль");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,1,1,6, Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Write_PCDate()));

    SysTime2 = new QLineEdit;
    timerRead = new QTimer(this);
    //connect(timerRead, SIGNAL(timeout()), this, SLOT(slot2_timeOut()));
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время в модуле:"), row,1,1,1);
    SysTime2->setText(Text);
    glyout->addWidget(SysTime2, row,2,1,4);

    /*row++;
    Button = new QPushButton("Читать время из модуля");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Start_Timer()));
    Button = new QPushButton("Остановить чтение");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,2,1,5,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Stop_Timer()));*/

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время для записи в модуль"), row,1,1,1);

    //for(int i = 0; i<3; i++)
    //{
    //glyout->addWidget(WDFunc::NewSPB(Date, "Date", 0, 100000, 0, paramcolor), row,2,1,1);
    glyout->addWidget(WDFunc::NewLE(this, "Date", SysTime2->text(), paramcolor), row,2,1,1);

    //}

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "день-месяц-год часы:минуты:секунды"), row,2,1,1);

    /*for(int i = 0; i<3; i++)
    {
       glyout->addWidget(WDFunc::NewSPB(this, "Time"+QString::number(i), 0, 59, 0, paramcolor), row,2+i,1,1);
    }*/


    row++;
    Button = new QPushButton("Записать заданное время в модуль");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,1,1,6, Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Write_Date()));

    //SysTime2->setText(QTime::currentTime().toString("hh:mm:ss"));
    // glyout->addWidget(SysTime2, row,2,1,4, Qt::AlignTop);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    time->setLayout(vlyout1);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);

    ConfTW->addTab(time,"Время");
    lyout->addWidget(ConfTW);
    setLayout(lyout);

}


void MNKTime::slot_timeOut()
{
   // WDFunc::LBLText(this, "adrMB", currentTime().toString("hh:mm:ss"));
    SysTime->setText(QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy HH:mm:ss"));
    //SysTime->update();
}

void MNKTime::slot2_timeOut()
{
    uint unixtimestamp = 0;
    QDateTime myDateTime;

    while (1)
    {
        if(!FinishThread)
        {
            if(MainWindow::MainInterface == "USB")
            {
                #if PROGSIZE != PROGSIZE_EMUL
                if (Commands::GetTimeMNK(unixtimestamp) == Error::ER_NOERROR)
                {
                  myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::UTC);
                  //myDateTime.setTime_t(unixtimestamp);
                  if(SysTime2 != nullptr)
                  SysTime2->setText(myDateTime.toString("dd-MM-yyyy HH:mm:ss"));
                  //WDFunc::SetTEData(this, "Date", SysTime2->text());
                }
                QThread::msleep(1000);

                #endif
            }
            else if(MainWindow::MainInterface == "Ethernet")
            {
               emit ethTimeRequest();
            }
            else if(MainWindow::MainInterface == "RS485")
            {
               emit modBusTimeRequest();
            }
        }

        if(closeThr) //&& (!haveFinished))
        {
         //haveFinished = true;
         emit finished();
         break;
        }
        QThread::msleep(10);
        qApp->processEvents();
        //break;
    }



}

void MNKTime::Start_Timer(int index)
{
    if(index == timeIndex)
    {
        if(MainWindow::MainInterface == "USB")
        {
            if (Commands::GetTimeMNK(unixtimestamp) == Error::ER_NOERROR)
            {
              QString qStr;
              myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::UTC);
              //myDateTime.setTime_t(unixtimestamp);
              if(SysTime2 != nullptr)
              SysTime2->setText(myDateTime.toString("dd-MM-yyyy HH:mm:ss"));
              qStr = SysTime2->text();
              WDFunc::LE_write_data(this, qStr, "Date");
            }
        }

        FinishThread = false;
    }

}

void MNKTime::Stop_Timer(int index)
{
    if(index != timeIndex)
    {
        FinishThread = true;
        //thr->msleep(100);
    }

}

void MNKTime::Write_PCDate()
{
    QDateTime myDateTime;
    uint time ;
    myDateTime = QDateTime::currentDateTimeUtc();
    time = myDateTime.toTime_t();


    #if PROGSIZE != PROGSIZE_EMUL
    FinishThread = true;
    TimeFunc::Wait(100);
    if (Commands::WriteTimeMNK(&time, sizeof(uint)) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");
    FinishThread = false;
    #endif

}

void MNKTime::Write_Date()
{
    QDateTime myDateTime;
    uint *time = new uint;
    QString qStr;
    WDFunc::LE_read_data(this, "Date", qStr);
    myDateTime = QDateTime::fromString(qStr,"dd-MM-yyyy HH:mm:ss");
    myDateTime.setOffsetFromUtc(0);
    *time = myDateTime.toTime_t();


    #if PROGSIZE != PROGSIZE_EMUL
    FinishThread = true;
    TimeFunc::Wait(100);
    if (Commands::WriteTimeMNK(time, sizeof(uint)) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");
    FinishThread = false;
    #endif

}

void MNKTime::StopSlot()
{
    FinishThread = true;
    closeThr = true;
}
