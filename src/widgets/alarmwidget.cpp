#include "alarmwidget.h"

#include "alarmbutton.h"
#include "wd_func.h"

#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QMenu>
#include <QtCore>

AlarmWidget::AlarmWidget(QWidget *parent) : QWidget(parent)
{

    QVBoxLayout *vlyout = new QVBoxLayout;

    QHBoxLayout *hlyout2 = new QHBoxLayout;
    QDialogButtonBox *buttons = new QDialogButtonBox;
    setMinimumWidth(parent->width() / 2);
    // buttons.addButton()
    // QStringList ButtonList { "AlarmButtonPressed", "ModuleWarnButtonPressed", "ModuleAlarmButtonPressed" };

    // for (int i = 0; i != buttonDescription.size(); i++)
    // {
    //        const QString text = buttonDescription.at(i);
    //        QHBoxLayout *hlyout = new QHBoxLayout;
    //        // QPushButton *pb = new QPushButton(text);
    //        QPushButton *pb = buttons->addButton(text, QDialogButtonBox::ActionRole);
    //        /*pb->setObjectName()*/ pb->setIcon(WDFunc::NewCircle(Qt::green, 15));
    //        pb->setMinimumSize(QSize(geometry().width() / 3, 30));
    //        int signal = metaObject()->indexOfSignal(QString((ButtonList.at(i) + "()")).toStdString().c_str());
    //        auto method = metaObject()->method(signal);
    //        connect(pb, QMetaMethod::fromSignal(&QAbstractButton::clicked), this, method);
    //        connect(pb, &QAbstractButton::clicked, [=] { buttonClicked(pb); });
    //        QGroupBox *gb = new QGroupBox("");
    //        hlyout->addWidget(pb, Qt::AlignRight);
    //        QPixmap map;
    //        hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(950 + i), &map), 1);
    //        WDFunc::SetVisible(this, QString::number(950 + i), false);
    //        gb->setLayout(hlyout);

    // buttons->addButton(*pb);
    // hlyout2->addWidget(gb);
    //}
    hlyout2->addWidget(buttons);
    //    QHBoxLayout *hlyout = new QHBoxLayout;
    //    QPixmap map;
    //    hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(953), &map), 1);
    //    hlyout2->addLayout(hlyout);

    //    QHBoxLayout *hlyout = new QHBoxLayout;
    //    QPixmap map;
    //    hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(953), &map, "Индикатор включения машины"), 1);
    //    hlyout2->addLayout(hlyout);

    // if (hlyout2->count())
    vlyout->addLayout(hlyout2);
    setLayout(vlyout);
    m_timer = new QTimer(this);
    m_timer->setInterval(10000);
    // qDebug() << static_cast<QLayout *>(layout()->children().first())->itemAt(0)->widget()->metaObject()->className();
    //    connect(Alarm, &AlarmClass::SetFirstButton, this, &AlarmWidget::UpdateFirstUSB);
    //    connect(Alarm, &AlarmClass::setWarnColor, this, &AlarmWidget::UpdateSecondUSB);
    //    connect(Alarm, &AlarmClass::setAlarmColor, this, &AlarmWidget::UpdateThirdUSB);
    //    connect(Alarm, &AlarmClass::SetIndicator, this, &AlarmWidget::UpdateIndicator);
}
// FIXME Переосмыслить индикатор
void AlarmWidget::UpdateIndicator(bool indx)
{
    auto pixmap
        = WDFunc::NewLedIndicator((indx == 0) ? QColor(Qt::green) : QColor(0xE0, 0xE0, 0xE0), this->height() / 2);
    // qDebug() << this->height();
    WDFunc::SetLBLImage(this, "953", &pixmap);
    WDFunc::SetVisible(this, "953", true);
}

// void AlarmWidget::update(bool w, bool a)
//{
//    UpdateFirstUSB();
//    UpdateSecondUSB(w);
//    UpdateThirdUSB(a);
//}

// void AlarmWidget::updateWarn(bool isset)
//{
//    auto pixmap = WDFunc::NewCircle((isset) ? Qt::yellow : Qt::green, this->height() / 4);
//    WDFunc::SetLBLImage(this, "951", &pixmap);
//    WDFunc::SetVisible(this, "951", true);
//}

// void AlarmWidget::updateAlarm(bool isset)
//{
//    auto pixmap = WDFunc::NewCircle((isset) ? Qt::red : Qt::green, this->height() / 4);
//    WDFunc::SetLBLImage(this, "952", &pixmap);
//    WDFunc::SetVisible(this, "952", true);
//}

// void AlarmWidget::updateMain(bool isset)
//{
//    Qt::GlobalColor color;
//    const auto &board = Board::GetInstance();
//    if (board.health() & BSIALARMMASK)
//        color = Qt::red;
//    else if (board.health() & BSIWARNMASK)
//        color = Qt::yellow;
//    else
//        color = Qt::green;
//    auto pixmap = WDFunc::NewCircle(color, this->height() / 4);
//    WDFunc::SetLBLImage(this, "950", &pixmap);
//    WDFunc::SetVisible(this, "950", true);
//}

// void AlarmWidget::updateMain(QColor color)
//{
//    auto pixmap = WDFunc::NewCircle(color, this->height() / 4);
//    WDFunc::SetLBLImage(this, "950", &pixmap);
//    WDFunc::SetVisible(this, "950", true);
//}

// void AlarmWidget::updateWarn(QColor color)
//{
//    auto pixmap = WDFunc::NewCircle(color, this->height() / 4);
//    WDFunc::SetLBLImage(this, "951", &pixmap);
//    WDFunc::SetVisible(this, "951", true);
//}

// void AlarmWidget::updateAlarm(QColor color)
//{
//    auto pixmap = WDFunc::NewCircle(color, this->height() / 4);
//    WDFunc::SetLBLImage(this, "952", &pixmap);
//    WDFunc::SetVisible(this, "952", true);
//}

// void AlarmWidget::buttonClicked(QWidget *button)
//{
//    qDebug() << button->metaObject()->className();
//}

// void AlarmWidget::UpdateSecondUSB(QList<bool> warnalarm)
// void AlarmWidget::UpdateSecondUSB(bool warn)
//{
//    //    // int i;
//    //    int alarm = 0;

//    //    // for(i=0; i<warnalarmcount.size(); i++)
//    //    foreach (bool item, warnalarm)
//    //    {
//    //        if (item == true)
//    //        {
//    //            alarm++;
//    //            break;
//    //        }
//    //    }
//    auto pixmap = WDFunc::NewCircle((warn) ? Qt::yellow : Qt::green, this->height() / 4);
//    WDFunc::SetLBLImage(this, "951", &pixmap);
//    WDFunc::SetVisible(this, "951", true);
//}

// void AlarmWidget::UpdateThirdUSB(QList<bool> avar)
// void AlarmWidget::UpdateThirdUSB(bool alarm)
//{
//    //    int alarm = 0;

//    //    //     for(i=0; i<alarmcount.size(); i++)
//    //    //    if(std::any_of(avar.constBegin(),avar.constEnd(),[](const bool state){return state;}))
//    //    //        alarm++;
//    //    foreach (bool item, avar)
//    //    {
//    //        if (item == true)
//    //        {
//    //            alarm++;
//    //            break;
//    //        }
//    //    }
//    auto pixmap = WDFunc::NewCircle((alarm) ? Qt::red : Qt::green, this->height() / 4);
//    WDFunc::SetLBLImage(this, "952", &pixmap);
//    WDFunc::SetVisible(this, "952", true);
//}

// void AlarmWidget::UpdateFirstUSB()
//{
//    Qt::GlobalColor color;
//    if (ModuleBSI::ModuleBsi.Hth & BSIALARMMASK)
//        color = Qt::red;
//    //    {
//    //        auto pixmap = WDFunc::NewCircle(Qt::red, this->height() / 4);
//    //        WDFunc::SetLBLImage(this, "950", &pixmap);
//    //        WDFunc::SetVisible(this, "950", true);
//    //    }
//    else if (ModuleBSI::ModuleBsi.Hth & BSIWARNMASK)
//        color = Qt::yellow;
//    //    {
//    //        auto pixmap = WDFunc::NewCircle(Qt::yellow, this->height() / 4);
//    //        WDFunc::SetLBLImage(this, "950", &pixmap);
//    //        WDFunc::SetVisible(this, "950", true);
//    //    }
//    else
//        color = Qt::green;
//    //    {
//    auto pixmap = WDFunc::NewCircle(color, this->height() / 4);
//    WDFunc::SetLBLImage(this, "950", &pixmap);
//    WDFunc::SetVisible(this, "950", true);
//    //    }
//}

void AlarmWidget::clear()
{
    //    WDFunc::SetVisible(this, "950", false);
    //    WDFunc::SetVisible(this, "951", false);
    //    WDFunc::SetVisible(this, "952", false);
    //    WDFunc::SetVisible(this, "953", false);
    // Имеются ли виджеты на слое
    m_timer->stop();
    Q_ASSERT(!layout()->children().isEmpty() && "Layout doesn't have children");
    QLayout *buttonBoxLayout = qobject_cast<QLayout *>(layout()->children().first());
    QDialogButtonBox *buttons = qobject_cast<QDialogButtonBox *>(buttonBoxLayout->itemAt(0)->widget());
    Q_ASSERT(buttons != nullptr);
    buttons->clear();
    qDeleteAll(m_alarms.begin(), m_alarms.end());
    m_alarms.clear();
    m_counter = 0;
}

// void AlarmWidget::uponInterfaceSetting()
//{
//}

void AlarmWidget::addAlarm(BaseAlarm *alarm)
{
    //    // Имеются ли виджеты на слое
    //    Q_ASSERT(!layout()->children().isEmpty() && "Layout doesn't have children");
    //    QLayout *buttonBoxLayout = qobject_cast<QLayout *>(layout()->children().first());
    //    QDialogButtonBox *buttons = qobject_cast<QDialogButtonBox *>(buttonBoxLayout->itemAt(0)->widget());
    //    Q_ASSERT(buttons != nullptr);
    //    AlarmButton *pb = new AlarmButton;
    //    // pb->setMinimumSize(QSize(3 * geometry().width() / 1, 30));
    //    // pb->setSizeIncrement()
    //    pb->setPixmap(WDFunc::NewCircle(Qt::green, 15));
    //    connect(pb, &QPushButton::clicked, alarm, &QDialog::show);
    //    //    alarm->setInterface(iface());
    //    if (!m_timer->isActive())
    //        m_timer->start();
    //    // temporarily disabled !!!    connect(m_timer, &QTimer::timeout, alarm, &BaseAlarm::reqUpdate);
    //    connect(alarm, &BaseAlarm::updateColor, pb, [=](QColor color) { pb->setPixmap(WDFunc::NewCircle(color, 15));
    //    });

    QString alarmName = alarm->metaObject()->className();
    if (alarmName.contains("warn", Qt::CaseInsensitive))
    {
        addAlarm(alarm, buttonDescription.at(1));
        //   pb->setText(buttonDescription.at(1));
    }
    if (alarmName.contains("crit", Qt::CaseInsensitive))
    {
        addAlarm(alarm, buttonDescription.at(2));
        //  pb->setText(buttonDescription.at(2));
    }
    if (alarmName.contains("stateall", Qt::CaseInsensitive))
    {
        addAlarm(alarm, buttonDescription.at(0));
        //  pb->setText(buttonDescription.at(0));
    }
    //    Q_ASSERT(!pb->text().isEmpty() && "Couldn't find description");
    //    m_alarms.append(alarm);
    //    buttons->addButton(pb, QDialogButtonBox::ActionRole);
    //    alarm->reqUpdate();
    //    ++m_counter;
}

void AlarmWidget::addAlarm(BaseAlarm *alarm, const QString caption)
{
    // Имеются ли виджеты на слое
    Q_ASSERT(!layout()->children().isEmpty() && "Layout doesn't have children");
    QLayout *buttonBoxLayout = qobject_cast<QLayout *>(layout()->children().first());
    QDialogButtonBox *buttons = qobject_cast<QDialogButtonBox *>(buttonBoxLayout->itemAt(0)->widget());
    Q_ASSERT(buttons != nullptr);
    AlarmButton *pb = new AlarmButton;
    // pb->setMinimumSize(QSize(3 * geometry().width() / 1, 30));
    // pb->setSizeIncrement()
    pb->setPixmap(WDFunc::NewCircle(Qt::green, 15));
    connect(pb, &QPushButton::clicked, alarm, &QDialog::show);
    //    alarm->setInterface(iface());
    if (!m_timer->isActive())
        m_timer->start();
    connect(m_timer, &QTimer::timeout, alarm, &BaseAlarm::reqUpdate);
    connect(alarm, &BaseAlarm::updateColor, pb, [=](QColor color) { pb->setPixmap(WDFunc::NewCircle(color, 15)); });

    pb->setText(caption);

    Q_ASSERT(!pb->text().isEmpty() && "Couldn't find description");
    m_alarms.append(alarm);
    buttons->addButton(pb, QDialogButtonBox::ActionRole);
    alarm->reqUpdate();
    ++m_counter;
}

int AlarmWidget::count() const
{
    return m_counter;
}

void AlarmWidget::disableAlarm()
{
    for (const auto &alarm : qAsConst(m_alarms))
        disconnect(m_timer, &QTimer::timeout, alarm, &BaseAlarm::reqUpdate);
    //   alarm->disable();
}
