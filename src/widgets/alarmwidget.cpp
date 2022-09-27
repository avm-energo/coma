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
    auto buttons = new QDialogButtonBox(this);
    auto vlyout = new QVBoxLayout(this);
    auto hlyout = new QHBoxLayout(buttons);
    setMinimumWidth(parent->width() / 2);
    hlyout->addWidget(buttons);
    vlyout->addLayout(hlyout);
    setLayout(vlyout);
    /// TODO: FIX THIS VLAD PLEASE
    m_timer = new QTimer(this);
    m_timer->setInterval(10000);
}

// FIXME Переосмыслить индикатор
void AlarmWidget::updateIndicator(bool indx)
{
    auto &&color = (indx) ? QColor(0xE0, 0xE0, 0xE0) : QColor(Qt::green);
    auto &&pixmap = WDFunc::NewLedIndicator(color, this->height() / 2);
    WDFunc::SetLBLImage(this, "953", &pixmap);
    WDFunc::SetVisible(this, "953", true);
}

void AlarmWidget::clear()
{
    // Имеются ли виджеты на слое
    m_timer->stop();
    if (!layout()->children().isEmpty())
    {
        auto buttonsLayout = qobject_cast<QLayout *>(layout()->children().first());
        auto buttons = qobject_cast<QDialogButtonBox *>(buttonsLayout->itemAt(0)->widget());
        if (buttons != nullptr)
        {
            buttons->clear();
            qDeleteAll(m_alarms.begin(), m_alarms.end());
            m_alarms.clear();
            m_counter = 0;
        }
    }
}

void AlarmWidget::addAlarm(BaseAlarm *alarm, const QString caption)
{
    // Имеются ли виджеты на слое
    if (!layout()->children().isEmpty())
    {
        auto buttonsLayout = qobject_cast<QLayout *>(layout()->children().first());
        auto buttons = qobject_cast<QDialogButtonBox *>(buttonsLayout->itemAt(0)->widget());
        if (buttons != nullptr)
        {
            alarm->setWindowTitle(caption);
            auto aButton = new AlarmButton(alarm);
            aButton->setPixmap(WDFunc::NewCircle(Qt::green, circleRadius));
            aButton->setText(caption);
            connect(aButton, &QPushButton::clicked, alarm, &BaseAlarm::show);
            connect(m_timer, &QTimer::timeout, alarm, &BaseAlarm::reqUpdate);
            connect(alarm, &BaseAlarm::updateColor, aButton, //
                [=](const QColor &color) { aButton->setPixmap(WDFunc::NewCircle(color, circleRadius)); });

            m_alarms.append(alarm);
            buttons->addButton(aButton, QDialogButtonBox::ActionRole);
            ++m_counter;
            if (!m_timer->isActive())
                m_timer->start();
        }
    }
}

int AlarmWidget::count() const
{
    return m_counter;
}

void AlarmWidget::disableAlarms()
{
    for (const auto &alarm : qAsConst(m_alarms))
        disconnect(m_timer, &QTimer::timeout, alarm, &BaseAlarm::reqUpdate);
}
