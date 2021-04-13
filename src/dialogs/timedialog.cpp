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
#include <QTimeZone>
#include <QTimer>
#include <QVBoxLayout>

constexpr char directOrder[] = "dd-MM-yyyy HH:mm:ss";
constexpr char reverseOrder[] = "yyyy-MM-ddTHH:mm:ss";

TimeDialog::TimeDialog(QWidget *parent) : UDialog(parent), First(false), Timer(new QTimer(this)), m_timeZone(DesktopTZ)
{
}

TimeDialog::~TimeDialog()
{
}

void TimeDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QWidget *time = new QWidget;

    auto *timeZoneLbl = WDFunc::NewLBL2(time, "Часовой пояс:");
    const QStringList cbl { "Местное время " + QString(QTimeZone::systemTimeZoneId()), "Время по Гринвичу" };
    auto *cb = WDFunc::NewCB2(time, detail::nameByValue(detail::Timezone), cbl);
    connect(cb, qOverload<int>(&QComboBox::currentIndexChanged), this, &TimeDialog::setTimeZone);
    auto *line = new QHBoxLayout;
    line->addWidget(timeZoneLbl);
    line->addWidget(cb);
    mainLayout->addLayout(line);

    auto *sysTimeText = WDFunc::NewLBL2(time, "Дата и время ПК:");
    auto *sysTimeVal = WDFunc::NewLBL2(
        time, QDateTime::currentDateTimeUtc().toString(reverseOrder), detail::nameByValue(detail::DesktopDatetime));
    connect(Timer, &QTimer::timeout, [=] {
        QString tmps;
        if (timeZone() == 0)
            tmps = QDateTime::currentDateTime().toString(directOrder);
        else
            tmps = QDateTime::currentDateTimeUtc().toString(directOrder);
        sysTimeVal->setText(tmps);
    });
    line = new QHBoxLayout;
    line->addWidget(sysTimeText);
    line->addWidget(sysTimeVal);
    mainLayout->addLayout(line);

    QPushButton *Button = new QPushButton("Записать дату и время ПК в модуль");
    connect(Button, &QAbstractButton::clicked, this, &TimeDialog::writePCDate);
    mainLayout->addWidget(Button);

    auto *moduleTimeText = WDFunc::NewLBL2(time, "Дата и время в модуле:");
    auto *moduleTimeVal = WDFunc::NewLBL2(time, directOrder, detail::nameByValue(detail::ModuleDatetime));
    line = new QHBoxLayout;
    line->addWidget(moduleTimeText);
    line->addWidget(moduleTimeVal);
    mainLayout->addLayout(line);

    auto *moduleWTimeText = WDFunc::NewLBL2(time, "Дата и время для записи в модуль");
    auto *moduleWTimeVal = WDFunc::NewLE2(time, detail::nameByValue(detail::WriteDatetime), directOrder);
    connect(this, &TimeDialog::timeZoneChanged, this, [moduleWTimeVal](TimeZone tz) {
        QDateTime myDateTime = QDateTime::fromString(moduleWTimeVal->text(), directOrder);
        if (tz == DesktopTZ)
        {
            myDateTime.setTimeZone(QTimeZone::utc());

            moduleWTimeVal->setText(myDateTime.toLocalTime().toString(directOrder));
        }
        else
        {
            myDateTime.setTimeZone(QTimeZone::systemTimeZone());
            moduleWTimeVal->setText(myDateTime.toUTC().toString(directOrder));
        }
    });

    moduleWTimeVal->setPlaceholderText("dd-MM-yyyy HH:mm:ss");
    // Неожиданно QLineEdit имеют отличную от QLabel default size policy
    moduleWTimeVal->setSizePolicy(moduleWTimeText->sizePolicy());
    moduleWTimeVal->setToolTip("день-месяц-год часы:минуты:секунды");
    line = new QHBoxLayout;
    line->addWidget(moduleWTimeText);
    line->addWidget(moduleWTimeVal);
    mainLayout->addLayout(line);

    Button = new QPushButton("Записать заданное время в модуль");
    connect(Button, &QAbstractButton::clicked, this, &TimeDialog::writeDate);
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

void TimeDialog::writePCDate()
{
    QDateTime myDateTime = QDateTime::currentDateTimeUtc();
    writeTime(myDateTime);
}

void TimeDialog::writeTime(QDateTime &myDateTime)
{
    uint time = myDateTime.toSecsSinceEpoch();
    BaseInterface::iface()->writeTime(time);
}

TimeDialog::TimeZone TimeDialog::timeZone() const
{
    return m_timeZone;
}

void TimeDialog::setTimeZone(int timeZone)
{
    m_timeZone = static_cast<TimeZone>(timeZone);
    emit timeZoneChanged(m_timeZone);
}

void TimeDialog::writeDate()
{
    QDateTime myDateTime
        = QDateTime::fromString(WDFunc::LEData(this, detail::nameByValue(detail::WriteDatetime)), directOrder);
    if (timeZone() == DesktopTZ)
        myDateTime.setTimeZone(QTimeZone::systemTimeZone());
    else
        myDateTime.setTimeZone(QTimeZone::utc());
    auto buffer = myDateTime.toUTC();
    writeTime(buffer);
}

void TimeDialog::uponInterfaceSetting()
{
    setupUI();
    connect(&DataManager::GetInstance(), &DataManager::bitStringReceived, this, &::TimeDialog::updateBitStringData);
}

void TimeDialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (bs.sigAdr              // USB doesnt know address so is empty
        && (bs.sigAdr != 4600) // other interfaces know address
    )
        return;
    setTime(bs.sigVal);
}

void TimeDialog::reqUpdate()
{
    if (updatesEnabled())
    {
        BaseInterface::iface()->reqTime();
    }
}

void TimeDialog::setTime(quint32 unixtimestamp)
{
    QDateTime myDateTime;

    if (timeZone() == DesktopTZ)
        myDateTime = QDateTime::fromSecsSinceEpoch(unixtimestamp, Qt::LocalTime);
    else
        myDateTime = QDateTime::fromSecsSinceEpoch(unixtimestamp, Qt::UTC);

    QString moduleTime = myDateTime.toString(directOrder);
    WDFunc::SetLBLText(this, detail::nameByValue(detail::ModuleDatetime), moduleTime);
    WDFunc::SetLEData(this, detail::nameByValue(detail::ModuleDatetime), moduleTime);
    if (First == 0)
    {
        WDFunc::SetLEData(this, detail::nameByValue(detail::WriteDatetime), moduleTime);
        First = 1;
    }
}

void TimeDialog::errorRead()
{

    WDFunc::SetLBLText(this, detail::nameByValue(detail::ModuleDatetime), "Ошибка чтения");
}
