#include "alarms/alarmwidget.h"

#include "../../widgets/wd_func.h"

#include <QDialogButtonBox>
#include <QTimer>
#include <alarms/alarmbutton.h>
#include <device/current_device.h>
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
void AlarmWidget::configure(Device::CurrentDevice *device)
{
    const static std::vector<std::pair<AlarmType, std::string_view>> alarmSettings {
        { AlarmType::Info, "Информационная сигнализация" },       //
        { AlarmType::Warning, "Предупредительная сигнализация" }, //
        { AlarmType::Critical, "Аварийная сигнализация" }         //
    };

    const auto &moduleSettings = device->getConfigStorage()->getDeviceSettings();
    const auto &alarmMap = moduleSettings.getAlarms();
    const auto &sigMap = moduleSettings.getSignals();

    auto alarmStateAll = new AlarmStateAll(device);
    QObject::connect(device, &Device::CurrentDevice::healthChanged, alarmStateAll, &AlarmStateAll::update);
    addAlarm(alarmStateAll, "Состояние устройства");
    for (const auto &[type, title] : alarmSettings)
    {
        const auto &alarms = alarmMap.value(type);
        if (!alarms.isEmpty())
        {
            auto moduleAlarm = new ModuleAlarm(type, alarms, device);
            moduleAlarm->followToData(sigMap);
            addAlarm(moduleAlarm, title.data());
        }
    }

    auto &userSettings = Settings::UserSettings::GetInstance();
    Settings::ScopedSettingsGroup _ { userSettings, "settings" };
    updateAlarmInterval(userSettings.get<Settings::AlarmsInterval>());
    updateAlarmOperation(userSettings.get<Settings::AlarmsEnabled>());
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
