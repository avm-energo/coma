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
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../gen/commands.h"
#include "../gen/error.h"
#include "../dialogs/time.h"
#include "../gen/timefunc.h"
#include "../gen/mainwindow.h"
#include "../gen/commands.h"

MNKTime::MNKTime(QWidget *parent) :
    QDialog(parent)
{
    //int i;

    FinishThread = true;
    closeThr = false;
    haveFinished = false;
    first = false;
    myDateTime.setTime_t(unixtimestamp);
    //setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    //thr->start();
}

MNKTime::~MNKTime()
{
}

void MNKTime::SetupUI()
{
    //QString phase[3] = {"Фаза A:","Фаза B:","Фаза C:"};
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

    glyout->addWidget(WDFunc::NewLBL(this, "Часовой пояс:"), row,1,1,1);
    QStringList cbl = QStringList() << "Местное время" << "Время по Гринвичу";
    EComboBox *cb = WDFunc::NewCB(this, "TimeZone", cbl, paramcolor);
    //cb->setMinimumWidth(80);
    cb->setMinimumHeight(20);
    glyout->addWidget(cb,row,2,1,4);
    row++;

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
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время в модуле:"), row,1,1,1);
    SysTime2->setText(Text);
    glyout->addWidget(SysTime2, row,2,1,4);
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время для записи в модуль"), row,1,1,1);
    glyout->addWidget(WDFunc::NewLE(this, "Date", SysTime2->text(), paramcolor), row,2,1,1);
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "день-месяц-год часы:минуты:секунды"), row,2,1,1);
    row++;
    Button = new QPushButton("Записать заданное время в модуль");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,1,1,6, Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Write_Date()));

    for(int i=0; i<6; i++)
    {
        glyout->addWidget(WDFunc::NewLBL(this, ""), row,0,1,1);
        row++;
    }
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
    int cbidx = WDFunc::CBIndex(this, "TimeZone");
    if(cbidx == 0)
    SysTime->setText(QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss"));
    else
    SysTime->setText(QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy HH:mm:ss"));

    //SysTime->update();
}

void MNKTime::slot2_timeOut()
{
    uint unixtimestamp = 0;
    QDateTime myDateTime;

    if(MainInterface == I_USB)
    {
        if (Commands::GetTimeMNK(unixtimestamp) == NOERROR)
        {
            int cbidx = WDFunc::CBIndex(this, "TimeZone");
            if(cbidx == 0)
                myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::LocalTime);
            else
                myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::UTC);
          if(SysTime2 != nullptr)
                SysTime2->setText(myDateTime.toString("dd-MM-yyyy HH:mm:ss"));
        }
    }
    else if(MainInterface == I_ETHERNET)
    {
       emit ethTimeRequest();
    }
    else if(MainInterface == I_RS485)
    {
       emit modBusTimeRequest();
    }
}

void MNKTime::Start_Timer(int index)
{
    int cbidx;
    if(index == timeIndex)
    {
        if(MainInterface == I_USB)
        {
            if (Commands::GetTimeMNK(unixtimestamp) == NOERROR)
            {
              QString qStr;
              cbidx = WDFunc::CBIndex(this, "TimeZone");
              if(cbidx == 0)
              myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::LocalTime);
              else
              myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::UTC);
              //myDateTime.setTime_t(unixtimestamp);
              if(SysTime2 != nullptr)
              SysTime2->setText(myDateTime.toString("dd-MM-yyyy HH:mm:ss"));
              qStr = SysTime2->text();
          WDFunc::LE_write_data(this, qStr, "Date");
            }
        }
        else
            first = 0;
    }
}

void MNKTime::Stop_Timer(int index)
{
    if(index != timeIndex)
        FinishThread = true;
}

void MNKTime::Write_PCDate()
{
    QDateTime myDateTime;
    uint time;
    int cbidx = WDFunc::CBIndex(this, "TimeZone");

    if(cbidx == 0)
    myDateTime = QDateTime::currentDateTime();
    else
    myDateTime = QDateTime::currentDateTimeUtc();

    time = myDateTime.toTime_t();

    if(MainInterface == I_USB)
    {
        //FinishThread = true;
        TimeFunc::Wait(100);
        if (Commands::WriteTimeMNK(time, sizeof(uint)) != NOERROR)
        EMessageBox::information(this, "INFO", "Ошибка"); //EMessageBox::information(this, "INFO", "Записано успешно");
        //FinishThread = false;
    }
    else if(MainInterface == I_ETHERNET)
      emit ethWriteTimeToModule(time);
    else if(MainInterface == I_RS485)
      emit modbusWriteTimeToModule(time);
}

void MNKTime::Write_Date()
{
    QDateTime myDateTime;
    QString qStr;
    WDFunc::LE_read_data(this, "Date", qStr);
    myDateTime = QDateTime::fromString(qStr,"dd-MM-yyyy HH:mm:ss");
    //int cbidx = WDFunc::CBIndex(this, "TimeZone");
    uint time = myDateTime.toTime_t();


    if(MainInterface == I_USB)
    {
        TimeFunc::Wait(100);
        if (Commands::WriteTimeMNK(time, sizeof(uint)) != NOERROR)
        EMessageBox::information(this, "INFO", "Ошибка"); //EMessageBox::information(this, "INFO", "Записано успешно");
    }
    else if(MainInterface == I_ETHERNET)
      emit ethWriteTimeToModule(time);
    else if(MainInterface == I_RS485)
      emit modbusWriteTimeToModule(time);
}

void MNKTime::StopSlot()
{
    FinishThread = true;
    closeThr = true;
}

void MNKTime::FillTimeFrom104(Parse104::BS104Signals* Time)
{
    uint unixtimestamp = 0;
    QString qStr;
    QDateTime myDateTime;
    int startadr = 0;
    memcpy(&startadr, &(Time->BS.SigAdr[0]), sizeof(Time->BS.SigAdr));

    if(startadr == 4600)
    {
       memcpy((quint32*)(&unixtimestamp), ((quint32*)(&Time->BS.SigVal)), sizeof(Time->BS.SigVal));
       int cbidx = WDFunc::CBIndex(this, "TimeZone");

       if(cbidx == 0)
       myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::LocalTime);
       else
       myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::UTC);

       if(SysTime2 != nullptr)
       SysTime2->setText(myDateTime.toString("dd-MM-yyyy HH:mm:ss"));

       if(first == 0)
       {
         qStr = SysTime2->text();
         WDFunc::LE_write_data(this, qStr, "Date");
         first = 1;
       }
    }
}

void MNKTime::FillTimeFromModBus(QList<ModBus::BSISignalStruct> Time)
{
    uint unixtimestamp = 0;
    QString qStr;
    QDateTime myDateTime;

    if (Time.size() == 0)
    {
        ERMSG("Некорректное время");
        DBGMSG;
        return;
    }
    if(Time.at(0).SigAdr == 4600)
    {
        unixtimestamp = Time.at(0).Val;
        int cbidx = WDFunc::CBIndex(this, "TimeZone");

        if(cbidx == 0)
        myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::LocalTime);
        else
        myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::UTC);

        if(SysTime2 != nullptr)
        SysTime2->setText(myDateTime.toString("dd-MM-yyyy HH:mm:ss"));

        if(first == 0)
        {
          qStr = SysTime2->text();
          WDFunc::LE_write_data(this, qStr, "Date");
          first = 1;
        }
    }
}

void MNKTime::ErrorRead()
{
    if(SysTime2 != nullptr)
        SysTime2->setText("Ошибка чтения");
}

void MNKTime::TimeWritten()
{
    EMessageBox::information(this, "Успешно", "Время записано успешно");
}
