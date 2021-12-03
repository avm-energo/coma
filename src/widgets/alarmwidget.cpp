#include "alarmwidget.h"

#include "../module/basealarm.h"
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

    hlyout2->addWidget(buttons);

    vlyout->addLayout(hlyout2);
    setLayout(vlyout);
    m_timer = new QTimer(this);
    m_timer->setInterval(10000);
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

void AlarmWidget::clear()
{

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

void AlarmWidget::addAlarm(BaseAlarm *alarm)
{

    QString alarmName = alarm->metaObject()->className();
    if (alarmName.contains("warn", Qt::CaseInsensitive))
    {
        addAlarm(alarm, buttonDescription.at(1));
    }
    if (alarmName.contains("crit", Qt::CaseInsensitive))
    {
        addAlarm(alarm, buttonDescription.at(2));
    }
    if (alarmName.contains("stateall", Qt::CaseInsensitive))
    {
        addAlarm(alarm, buttonDescription.at(0));
    }
}

void AlarmWidget::addAlarm(BaseAlarm *alarm, const QString caption)
{
    // Имеются ли виджеты на слое
    Q_ASSERT(!layout()->children().isEmpty() && "Layout doesn't have children");
    QLayout *buttonBoxLayout = qobject_cast<QLayout *>(layout()->children().first());
    QDialogButtonBox *buttons = qobject_cast<QDialogButtonBox *>(buttonBoxLayout->itemAt(0)->widget());
    Q_ASSERT(buttons != nullptr);
    alarm->setWindowTitle(caption);
    AlarmButton *pb = new AlarmButton(alarm);

    pb->setPixmap(WDFunc::NewCircle(Qt::green, 15));
    connect(pb, &QPushButton::clicked, alarm, &QDialog::show);

    if (!m_timer->isActive())
        m_timer->start();
    connect(m_timer, &QTimer::timeout, alarm, &BaseAlarm::reqUpdate);
    connect(alarm, &BaseAlarm::updateColor, pb, [=](QColor color) { pb->setPixmap(WDFunc::NewCircle(color, 15)); });

    pb->setText(caption);

    Q_ASSERT(!pb->text().isEmpty() && "Couldn't find description");
    m_alarms.append(alarm);
    buttons->addButton(pb, QDialogButtonBox::ActionRole);
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
