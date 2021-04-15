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

TimeDialog::TimeDialog(QWidget *parent) : UDialog(parent), First(false), Timer(new QTimer(this))
{
}

TimeDialog::~TimeDialog()
{
}

void TimeDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QWidget *time = new QWidget;

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(new QLabel("Часовой пояс", this));
    hlyout->addWidget(new QLabel("UTC", this));
    hlyout->addWidget(new QLabel(TimeFunc::userTimeZoneName(), this));
    mainLayout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(new QLabel("Дата и время:", this));
    auto *label = new QLabel(this);
    connect(Timer, &QTimer::timeout, [=] {
        auto datetime = QDateTime::currentDateTimeUtc();
        label->setText(datetime.toString(directOrder));
    });
    hlyout->addWidget(label);
    label = WDFunc::NewLBL2(
        time, QDateTime::currentDateTimeUtc().toString(reverseOrder), settings::nameByValue(settings::DesktopDatetime));
    connect(Timer, &QTimer::timeout, [=] {
        auto datetime = QDateTime::currentDateTimeUtc().toTimeZone(TimeFunc::userTimeZone());
        label->setText(datetime.toString(directOrder));
    });
    hlyout->addWidget(label);
    mainLayout->addLayout(hlyout);
    hlyout = new QHBoxLayout;

    QPushButton *Button = new QPushButton("Записать время (UTC) ПК в модуль");
    connect(Button, &QAbstractButton::clicked, this, &TimeDialog::writePCDate);

    mainLayout->addWidget(Button);

    label = WDFunc::NewLBL2(time, "Дата и время в модуле:");
    hlyout->addWidget(label);

    label = WDFunc::NewLBL2(time, directOrder, settings::nameByValue(settings::ModuleDatetimeUtc));
    hlyout->addWidget(label);

    label = WDFunc::NewLBL2(time, directOrder, settings::nameByValue(settings::ModuleDatetime));
    hlyout->addWidget(label);

    mainLayout->addLayout(hlyout);
    hlyout = new QHBoxLayout;

    label = WDFunc::NewLBL2(time, "Дата и время для записи в модуль");
    hlyout->addWidget(label);

    auto *lineedit = WDFunc::NewLE2(time, settings::nameByValue(settings::WriteDatetime), directOrder);
    lineedit->setPlaceholderText("dd-MM-yyyy HH:mm:ss");
    lineedit->setClearButtonEnabled(true);
    lineedit->setToolTip("день-месяц-год часы:минуты:секунды");
    lineedit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(lineedit, &QLineEdit::textChanged, this, [lineedit](const QString &str) {
        if (str.isEmpty())
        {
            auto datetime = QDateTime::currentDateTimeUtc().toTimeZone(TimeFunc::userTimeZone());
            lineedit->setText(datetime.toString(directOrder));
        }
    });

    label = new QLabel(this);
    connect(lineedit, &QLineEdit::textChanged, label, [label](const QString &str) {
        if (!str.isEmpty())
        {
            auto datetime = QDateTime::fromString(str, directOrder);
            datetime.setTimeZone(TimeFunc::userTimeZone());
            auto timeStr = datetime.toUTC().toString(directOrder);
            label->setText(timeStr.isEmpty() ? "Invalid" : timeStr);
        }
    });
    hlyout->addWidget(label);
    hlyout->addWidget(lineedit);

    mainLayout->addLayout(hlyout);
    Button = new QPushButton("Записать заданное время в модуль");
    mainLayout->addWidget(Button);
    connect(Button, &QAbstractButton::clicked, this, &TimeDialog::writeDate);

    mainLayout->setAlignment(Qt::AlignVCenter);
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

void TimeDialog::writeDate()
{
    QDateTime myDateTime
        = QDateTime::fromString(WDFunc::LEData(this, settings::nameByValue(settings::WriteDatetime)), directOrder);
    myDateTime.setTimeZone(TimeFunc::userTimeZone());

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
    QDateTime myDateTime = QDateTime::fromSecsSinceEpoch(unixtimestamp, TimeFunc::userTimeZone());
    QString moduleTime = myDateTime.toString(directOrder);
    QString moduleTimeUtc = myDateTime.toUTC().toString(directOrder);
    WDFunc::SetLBLText(this, settings::nameByValue(settings::ModuleDatetime), moduleTime);
    WDFunc::SetLBLText(this, settings::nameByValue(settings::ModuleDatetimeUtc), moduleTimeUtc);
    if (First == 0)
    {
        WDFunc::SetLEData(this, settings::nameByValue(settings::WriteDatetime), moduleTime);
        First = 1;
    }
}

void TimeDialog::errorRead()
{

    WDFunc::SetLBLText(this, settings::nameByValue(settings::ModuleDatetime), "Ошибка чтения");
}
