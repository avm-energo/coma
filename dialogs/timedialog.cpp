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
    setTimeZone(0);
    First = false;
    Timer = new QTimer(this);
}

TimeDialog::~TimeDialog()
{
}

void TimeDialog::SetupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QWidget *time = new QWidget;

    auto *timeZoneLbl = WDFunc::NewLBL2(time, "Часовой пояс:");
    const QStringList cbl { "Местное время", "Время по Гринвичу" };
    auto *cb = WDFunc::NewCB2(time, cbl);
    connect(cb, qOverload<int>(&QComboBox::currentIndexChanged), this, &TimeDialog::setTimeZone);
    auto *line = new QHBoxLayout;
    line->addWidget(timeZoneLbl);
    line->addWidget(cb);
    mainLayout->addLayout(line);

    auto *sysTimeText = WDFunc::NewLBL2(time, "Дата и время ПК:");
    auto *sysTimeVal = WDFunc::NewLBLT2(time, QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ss"));
    connect(Timer, &QTimer::timeout, [=] {
        QString tmps;
        if (timeZone() == 0)
            tmps = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss");
        else
            tmps = QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy HH:mm:ss");
        sysTimeVal->setText(tmps);
    });
    line = new QHBoxLayout;
    line->addWidget(sysTimeText);
    line->addWidget(sysTimeVal);
    mainLayout->addLayout(line);

    QPushButton *Button = new QPushButton("Записать дату и время ПК в модуль");
    connect(Button, &QAbstractButton::clicked, this, &TimeDialog::Write_PCDate);
    mainLayout->addWidget(Button);

    auto *moduleTimeText = WDFunc::NewLBL2(time, "Дата и время в модуле:");
    auto *moduleTimeVal = WDFunc::NewLBL2(time, "dd-MM-yyyy HH:mm:ss", "moduleTime");
    line = new QHBoxLayout;
    line->addWidget(moduleTimeText);
    line->addWidget(moduleTimeVal);
    mainLayout->addLayout(line);

    auto *moduleWTimeText = WDFunc::NewLBL2(time, "Дата и время для записи в модуль");
    auto *moduleWTimeVal = WDFunc::NewLE2(time, "Date", "dd-MM-yyyy HH:mm:ss" /*, paramcolor*/);
    // Неожиданно QLineEdit имеют отличную от QLabel default size policy
    moduleWTimeVal->setSizePolicy(moduleWTimeText->sizePolicy());
    moduleWTimeVal->setToolTip("день-месяц-год часы:минуты:секунды");
    line = new QHBoxLayout;
    line->addWidget(moduleWTimeText);
    line->addWidget(moduleWTimeVal);
    mainLayout->addLayout(line);

    Button = new QPushButton("Записать заданное время в модуль");
    connect(Button, &QAbstractButton::clicked, this, &TimeDialog::Write_Date);
    mainLayout->addWidget(Button);
    mainLayout->addSpacing(height() / 2);

    time->setLayout(mainLayout);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");

    ConfTW->addTab(time, "Время");
    lyout->addWidget(ConfTW);
    setLayout(lyout);
    Timer->start(1000);
}

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
    if (updatesEnabled())
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
