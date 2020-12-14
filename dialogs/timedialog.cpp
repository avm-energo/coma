#include "../dialogs/timedialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"

#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>

TimeDialog::TimeDialog(QWidget *parent) : UDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    First = false;
    Timer = new QTimer(this);
}

TimeDialog::~TimeDialog()
{
}

void TimeDialog::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    //    QWidget *analog1 = new QWidget;
    //    QWidget *analog2 = new QWidget;
    //    QWidget *extraconf = new QWidget;
    //    QWidget *MEKconf = new QWidget;
    QWidget *time = new QWidget;
    // QString tmps = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
    // analog1->setStyleSheet(tmps);
    // analog2->setStyleSheet(tmps);
    // extraconf->setStyleSheet(tmps);
    // time->setStyleSheet(tmps);
    // MEKconf->setStyleSheet(tmps);
    int row = 0;

    // QString paramcolor = Colors::MAINWINCLR;

    QGroupBox *gb = new QGroupBox;

    glyout->addWidget(WDFunc::NewLBL2(this, "Часовой пояс:"), row, 1, 1, 1);
    const QStringList cbl { "Местное время", "Время по Гринвичу" };
    auto *cb = WDFunc::NewCB2(this, cbl /*, paramcolor*/);
    connect(cb, qOverload<int>(&QComboBox::currentIndexChanged), this, &TimeDialog::setTimeZone);
    // cb->setMinimumWidth(80);zzz
    cb->setMinimumHeight(20);
    glyout->addWidget(cb, row, 2, 1, 4);
    row++;

    glyout->addWidget(WDFunc::NewLBL2(this, "Дата и время ПК:"), row, 1, 1, 1, Qt::AlignTop);
    auto *lbl = WDFunc::NewLBLT2(this, QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ss"));
    connect(Timer, &QTimer::timeout, [=] {
        QString tmps;
        if (timeZone() == 0)
            tmps = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss");
        else
            tmps = QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy HH:mm:ss");
        lbl->setText(tmps);
    });
    glyout->addWidget(lbl, row++, 2, 1, 4, Qt::AlignTop);
    // tmps = "QWidget {background-color: " + QString(Colors::MAINWINCLR) + ";}";
    QPushButton *Button = new QPushButton("Записать дату и время ПК в модуль");
    // Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row++, 1, 1, 6, Qt::AlignTop);
    connect(Button, &QAbstractButton::clicked, this, &TimeDialog::Write_PCDate);
    glyout->addWidget(WDFunc::NewLBL2(this, "Дата и время в модуле:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "dd-MM-yyyy HH:mm:ss", "moduleTime"), row++, 2, 1, 4);
    glyout->addWidget(WDFunc::NewLBL2(this, "Дата и время для записи в модуль"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLE2(this, "Date", "dd-MM-yyyy HH:mm:ss" /*, paramcolor*/), row++, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "день-месяц-год часы:минуты:секунды"), row++, 2, 1, 1);
    Button = new QPushButton("Записать заданное время в модуль");
    // Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row, 1, 1, 6, Qt::AlignTop);
    connect(Button, &QAbstractButton::clicked, this, &TimeDialog::Write_Date);

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
    // QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::TABCOLOR) + ";}";
    // ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(time, "Время");
    lyout->addWidget(ConfTW);
    setLayout(lyout);
    Timer->start(1000);
}

// void TimeDialog::updateSysTime()
//{
//    QString tmps;
//    if (timeZone() == 0)
//        tmps = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss");
//    else
//        tmps = QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy HH:mm:ss");
//    WDFunc::SetLBLText(this, "systime", tmps);
//}

// void TimeDialog::USBUpdate()
//{

//    switch (Board::GetInstance().interfaceType())
//    {
//    case Board::InterfaceType::USB:
//    {
//        uint unixtimestamp = 0;
//        if (Commands::GetTimeMNK(unixtimestamp) == Error::Msg::NoError)
//            SetTime(unixtimestamp);
//        break;
//    }
//    case Board::InterfaceType::Ethernet:
//        emit ethTimeRequest();
//        break;
//    case Board::InterfaceType::RS485:
//        emit modBusTimeRequest();
//        break;
//    }
//}

void TimeDialog::Write_PCDate()
{
    QDateTime myDateTime;
    if (timeZone() == 0)
        myDateTime = QDateTime::currentDateTime();
    else
        myDateTime = QDateTime::currentDateTimeUtc();
    WriteTime(myDateTime);
}

void TimeDialog::WriteTime(QDateTime &myDateTime)
{
    uint time = myDateTime.toSecsSinceEpoch();
    BaseInterface::iface()->writeTime(time);
    //    switch (Board::GetInstance().interfaceType())
    //    {
    //    case Board::InterfaceType::USB:
    //    {
    //        TimeFunc::Wait(100);
    //        if (Commands::WriteTimeMNK(time, sizeof(uint)) != Error::Msg::NoError)
    //            QMessageBox::information(this, "INFO",
    //                "Ошибка"); // QMessageBox::information(this,
    //                           // "INFO", "Записано успешно");
    //        break;
    //    }
    //    case Board::InterfaceType::Ethernet:
    //        emit ethWriteTimeToModule(time);
    //        break;
    //    case Board::InterfaceType::RS485:
    //        emit modbusWriteTimeToModule(time);
    //        break;
    //    }
}

int TimeDialog::timeZone() const
{
    return m_timeZone;
}

void TimeDialog::setTimeZone(int timeZone)
{
    m_timeZone = timeZone;
}

void TimeDialog::Write_Date()
{
    QDateTime myDateTime = QDateTime::fromString(WDFunc::LEData(this, "Date"), "dd-MM-yyyy HH:mm:ss");
    WriteTime(myDateTime);
}

void TimeDialog::uponInterfaceSetting()
{
    SetupUI();
    connect(&DataManager::GetInstance(), &DataManager::bitStringReceived, this, &::TimeDialog::updateBitStringData);
}

void TimeDialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    // Time doesnt have address
    // USB +
    if (bs.sigAdr)
        return;
    SetTime(bs.sigVal);
}

// void TimeDialog::ETHUpdate()
//{
// QString qStr;
// QDateTime myDateTime;
//    int startadr = 0;
//    memcpy(&startadr, &(Time->BS.SigAdr[0]), sizeof(Time->BS.SigAdr));

//    if (startadr == MBS_TIMEREG)
//    {
//        memcpy((quint32 *)(&unixtimestamp), ((quint32 *)(&Time->BS.SigVal)), sizeof(Time->BS.SigVal));
//        SetTime(unixtimestamp);
//    }
//}

void TimeDialog::reqUpdate()
{
    if (m_updatesEnabled)
    {
        BaseInterface::iface()->reqTime();
        //        uint unixtimestamp = 0;
        //        QList<DataTypes::SignalsStruct> list;
        //        // DataManager::getSignals(TIMEREG, TIMEREG, DataTypes::SignalTypes::BitString, list);
        //        if (!list.isEmpty())
        //        {
        //            for (auto signal : list)
        //            {
        //                DataTypes::BitStringStruct bs = qvariant_cast<DataTypes::BitStringStruct>(signal.data);
        //                memcpy(&unixtimestamp, &bs.sigVal, sizeof(quint32));
        //                SetTime(unixtimestamp);
        //            }
        //        }
        //        // send command to get time
        //        switch (Board::GetInstance().interfaceType())
        //        {
        //        case Board::InterfaceType::USB:
        //            USBUpdate();
        //            break;
        //        case Board::InterfaceType::Ethernet:
        //            ETHUpdate();
        //            break;
        //        case Board::InterfaceType::RS485:
        //            MBSUpdate();
        //            break;
        //        default:
        //            break;
        //        }
    }
}

void TimeDialog::SetTime(quint32 unixtimestamp)
{
    QDateTime myDateTime;

    if (timeZone() == 0)
        myDateTime = QDateTime::fromSecsSinceEpoch(unixtimestamp, Qt::LocalTime);
    else
        myDateTime = QDateTime::fromSecsSinceEpoch(unixtimestamp, Qt::UTC);

    QString moduleTime = myDateTime.toString("dd-MM-yyyy HH:mm:ss");
    WDFunc::SetLBLText(this, "moduleTime", moduleTime);
    WDFunc::SetLEData(this, "moduleTime", moduleTime);
    if (First == 0)
    {
        WDFunc::SetLEData(this, "Date", moduleTime);
        First = 1;
    }
}

// void TimeDialog::MBSUpdate(QList<ModBus::BSISignalStruct> Time)
// void TimeDialog::MBSUpdate()
//{
//    uint unixtimestamp = 0;

//    /*    if (Time.size() == 0)
//        {
//            ERMSG("Некорректное время");
//            return;
//        }
//        if (Time.at(0).SigAdr == MBS_TIMEREG)
//        {
//            unixtimestamp = Time.at(0).Val;
//            SetTime(unixtimestamp);
//        } */
//}

void TimeDialog::ErrorRead()
{

    WDFunc::SetLBLText(this, "moduleTime", "Ошибка чтения");
}

void TimeDialog::TimeWritten()
{
    QMessageBox::information(this, "Успешно", "Время записано успешно");
}
