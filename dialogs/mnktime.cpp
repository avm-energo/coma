#include "../dialogs/mnktime.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../gen/timefunc.h"
#include "../usb/commands.h"
#include "../widgets/ecombobox.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>

MNKTime::MNKTime(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    First = false;
    Timer = new QTimer(this);
    connect(Timer, SIGNAL(timeout()), this, SLOT(slot_timeOut()));
    SetupUI();
}

MNKTime::~MNKTime() { }

void MNKTime::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *extraconf = new QWidget;
    QWidget *MEKconf = new QWidget;
    QWidget *time = new QWidget;
    QString tmps = "QWidget {background-color: " + QString(ACONFWCLR) + ";}";
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

    glyout->addWidget(WDFunc::NewLBL(this, "Часовой пояс:"), row, 1, 1, 1);
    QStringList cbl = QStringList() << "Местное время"
                                    << "Время по Гринвичу";
    EComboBox *cb = WDFunc::NewCB(this, "TimeZone", cbl, paramcolor);
    // cb->setMinimumWidth(80);
    cb->setMinimumHeight(20);
    glyout->addWidget(cb, row, 2, 1, 4);
    row++;

    QDateTime current = QDateTime::currentDateTime();
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время ПК:"), row, 1, 1, 1, Qt::AlignTop);
    //    QDateTime dt = QDateTime::currentDateTime();
    glyout->addWidget(WDFunc::NewLBLT(this, QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ss"), "systime"),
        row++, 2, 1, 4, Qt::AlignTop);
    tmps = "QWidget {background-color: " + QString(MAINWINCLR) + ";}";
    QPushButton *Button = new QPushButton("Записать дату и время ПК в модуль");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row++, 1, 1, 6, Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Write_PCDate()));
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время в модуле:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "systime2", "dd-MM-yyyy HH:mm:ss"), row++, 2, 1, 4);
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время для записи в модуль"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "Date", "dd-MM-yyyy HH:mm:ss", paramcolor), row++, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "день-месяц-год часы:минуты:секунды"), row++, 2, 1, 1);
    Button = new QPushButton("Записать заданное время в модуль");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row, 1, 1, 6, Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Write_Date()));

    for (int i = 0; i < 6; i++)
    {
        glyout->addWidget(WDFunc::NewLBL(this, ""), row, 0, 1, 1);
        row++;
    }
    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    time->setLayout(vlyout1);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(TABCOLOR) + ";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(time, "Время");
    lyout->addWidget(ConfTW);
    setLayout(lyout);
    Timer->start(1000);
}

void MNKTime::slot_timeOut()
{
    QString tmps;
    int cbidx = WDFunc::CBIndex(this, "TimeZone");
    if (cbidx == 0)
        tmps = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss");
    else
        tmps = QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy HH:mm:ss");
    WDFunc::SetLBLText(this, "systime", tmps);
}

void MNKTime::slot2_timeOut()
{
    uint unixtimestamp = 0;
    QDateTime myDateTime;

    if (MainInterface == I_USB)
    {
        if (Commands::GetTimeMNK(unixtimestamp) == NOERROR)
            SetTime(unixtimestamp);
    }
    else if (MainInterface == I_ETHERNET)
        emit ethTimeRequest();
    else if (MainInterface == I_RS485)
        emit modBusTimeRequest();
}

void MNKTime::Write_PCDate()
{
    QDateTime myDateTime;
    int cbidx = WDFunc::CBIndex(this, "TimeZone");
    if (cbidx == 0)
        myDateTime = QDateTime::currentDateTime();
    else
        myDateTime = QDateTime::currentDateTimeUtc();
    WriteTime(myDateTime);
}

void MNKTime::WriteTime(QDateTime &myDateTime)
{
    uint time = myDateTime.toSecsSinceEpoch();
    if (MainInterface == I_USB)
    {
        TimeFunc::Wait(100);
        if (Commands::WriteTimeMNK(time, sizeof(uint)) != NOERROR)
            EMessageBox::information(
                this, "INFO", "Ошибка"); // EMessageBox::information(this, "INFO", "Записано успешно");
    }
    else if (MainInterface == I_ETHERNET)
        emit ethWriteTimeToModule(time);
    else if (MainInterface == I_RS485)
        emit modbusWriteTimeToModule(time);
}

void MNKTime::Write_Date()
{
    QDateTime myDateTime = QDateTime::fromString(WDFunc::LEData(this, "Date"), "dd-MM-yyyy HH:mm:ss");
    WriteTime(myDateTime);
}

void MNKTime::FillTimeFrom104(IEC104Thread::BS104Signals *Time)
{
    uint unixtimestamp = 0;
    QString qStr;
    QDateTime myDateTime;
    int startadr = 0;
    memcpy(&startadr, &(Time->BS.SigAdr[0]), sizeof(Time->BS.SigAdr));

    if (startadr == 4600)
    {
        memcpy((quint32 *)(&unixtimestamp), ((quint32 *)(&Time->BS.SigVal)), sizeof(Time->BS.SigVal));
        SetTime(unixtimestamp);
    }
}

void MNKTime::SetTime(quint32 unixtimestamp)
{
    QDateTime myDateTime;
    int cbidx = WDFunc::CBIndex(this, "TimeZone");

    if (cbidx == 0)
        myDateTime = QDateTime::fromSecsSinceEpoch(unixtimestamp, Qt::LocalTime);
    else
        myDateTime = QDateTime::fromSecsSinceEpoch(unixtimestamp, Qt::UTC);

    QString systime2 = myDateTime.toString("dd-MM-yyyy HH:mm:ss");
    WDFunc::SetLEData(this, "systime2", systime2);
    if (First == 0)
    {
        WDFunc::SetLEData(this, "Date", systime2);
        First = 1;
    }
}

void MNKTime::FillTimeFromModBus(QList<ModBus::BSISignalStruct> Time)
{
    uint unixtimestamp = 0;

    if (Time.size() == 0)
    {
        ERMSG("Некорректное время");
        DBGMSG;
        return;
    }
    if (Time.at(0).SigAdr == 4600)
    {
        unixtimestamp = Time.at(0).Val;
        SetTime(unixtimestamp);
    }
}

void MNKTime::ErrorRead() { WDFunc::SetLEData(this, "systime2", "Ошибка чтения"); }

void MNKTime::TimeWritten() { EMessageBox::information(this, "Успешно", "Время записано успешно"); }
