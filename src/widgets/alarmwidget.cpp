#include "alarmwidget.h"

#include "../module/configstorage.h"
#include "alarmbutton.h"
#include "wd_func.h"

#include <QDialogButtonBox>
#include <QTimer>

AlarmWidget::AlarmWidget(QWidget *parent) : QWidget(parent)
{
    auto buttons = new QDialogButtonBox(this);
    auto vlyout = new QVBoxLayout(this);
    auto hlyout = new QHBoxLayout;
    setMinimumWidth(parent->width() / 2);
    hlyout->addWidget(buttons);
    vlyout->addLayout(hlyout);
    setLayout(vlyout);
    m_timer = new QTimer(this);
    m_timer->setInterval(10000);
}

/// \brief Filling alarms in this alarm widget.
void AlarmWidget::configure()
{
    static const QList<QPair<ModuleTypes::AlarmKey, QString>> alarmSettings {
        { Modules::AlarmType::Info, "Информационная сигнализация" },       //
        { Modules::AlarmType::Warning, "Предупредительная сигнализация" }, //
        { Modules::AlarmType::Critical, "Аварийная сигнализация" }         //
    };

    auto alarmStateAll = new AlarmStateAll();
    alarmStateAll->setupUI(AVM::HthToolTip);
    addAlarm(alarmStateAll, "Состояние устройства");
    const auto &alarmMap = ConfigStorage::GetInstance().getModuleSettings().getAlarms();

    for (auto &pair : alarmSettings)
    {
        const auto &title = pair.second;
        const auto &alarms = alarmMap.value(pair.first);
        if (!alarms.isEmpty())
        {
            auto moduleAlarm = new ModuleAlarm(pair.first, alarms);
            addAlarm(moduleAlarm, title);
        }
    }

    // Start the timer after all module alarms was initialized
    if (!m_timer->isActive())
        m_timer->start();
}

/// \brief Adding a received alarm in list and creating a button for a received alarm.
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
            aButton->setPixmap(WDFunc::NewCircle(Qt::transparent, circleRadius));
            aButton->setText(caption);
            connect(aButton, &QPushButton::clicked, alarm, &BaseAlarm::show);
            connect(m_timer, &QTimer::timeout, alarm, &BaseAlarm::reqUpdate);
            connect(alarm, &BaseAlarm::updateColor, aButton,                    //
                [=](const QColor &color) {                                      //
                    aButton->setPixmap(WDFunc::NewCircle(color, circleRadius)); //
                });                                                             //

            m_alarms.append(alarm);
            buttons->addButton(aButton, QDialogButtonBox::ActionRole);
            ++m_counter;
        }
    }
}

/// \brief Disable all requests for alarms in list.
void AlarmWidget::disableAlarms()
{
    for (const auto &alarm : qAsConst(m_alarms))
        disconnect(m_timer, &QTimer::timeout, alarm, &BaseAlarm::reqUpdate);
}

/// \brief Delete all alarms from the list.
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
