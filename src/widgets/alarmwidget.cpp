#include "alarmwidget.h"

#include "../module/configstorage.h"
#include "alarmbutton.h"
#include "wd_func.h"

#include <QDialogButtonBox>
#include <QTimer>
#include <settings/user_settings.h>

AlarmWidget::AlarmWidget(QWidget *parent) : QWidget(parent), m_timer(new QTimer(this))
{
    auto buttons = new QDialogButtonBox(this);
    auto vlyout = new QVBoxLayout(this);
    auto hlyout = new QHBoxLayout;
    setMinimumWidth(parent->width() / 2);
    hlyout->addWidget(buttons);
    vlyout->addLayout(hlyout);
    setLayout(vlyout);
}

/// \brief Filling alarms in this alarm widget.
void AlarmWidget::configure()
{
    const static std::vector<std::pair<Modules::AlarmType, std::string_view>> alarmSettings {
        { Modules::AlarmType::Info, "Информационная сигнализация" },       //
        { Modules::AlarmType::Warning, "Предупредительная сигнализация" }, //
        { Modules::AlarmType::Critical, "Аварийная сигнализация" }         //
    };

    auto alarmStateAll = new AlarmStateAll();
    alarmStateAll->setupUI(AVM::HthToolTip);
    addAlarm(alarmStateAll, "Состояние устройства");
    const auto &alarmMap = ConfigStorage::GetInstance().getModuleSettings().getAlarms();

    for (const auto &[type, title] : alarmSettings)
    {
        const auto &alarms = alarmMap.value(type);
        if (!alarms.isEmpty())
        {
            auto moduleAlarm = new ModuleAlarm(type, alarms);
            addAlarm(moduleAlarm, title.data());
        }
    }

    auto &settings = Settings::UserSettings::GetInstance();
    Settings::ScopedSettingsGroup _ { settings, "settings" };
    updateAlarmInterval(settings.get<Settings::AlarmsInterval>());
    updateAlarmOperation(settings.get<Settings::AlarmsEnabled>());
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

/// \brief Slot for updating alarm operation mode.
void AlarmWidget::updateAlarmOperation(const bool mode) noexcept
{
    if (mode)
    {
        if (!m_timer->isActive())
            m_timer->start();
    }
    else
    {
        /// TODO: здесь можно очищать виджеты сигнализации
        if (m_timer->isActive())
            m_timer->stop();
    }
}

/// \brief Slot for updating alarms request interval.
void AlarmWidget::updateAlarmInterval(const int interval) noexcept
{
    m_timer->setInterval(interval);
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
