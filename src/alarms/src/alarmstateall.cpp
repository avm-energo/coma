#include "alarms/alarmstateall.h"

#include <QBoxLayout>
#include <bitset>
#include <device/current_device.h>
#include <widgets/wd_func.h>

AlarmStateAll::AlarmStateAll(Device::CurrentDevice *device, QWidget *parent)
    : BaseAlarm(device, parent)
    , m_config(device->getConfigStorage()->getDeviceSettings().getStateAllConfig())
    , m_strongestColor(Qt::transparent)
{
    if (m_config.empty())
        generateDefaultConfig();
    setupUI();
}

void AlarmStateAll::generateDefaultConfig()
{
    m_config = AlarmStateAllConfig {
        { 0, AlarmType::Warning, "Проблемы со встроенным АЦП" },                     //
        { 1, AlarmType::Warning, "Не работает внешняя flash-память" },               //
        { 2, AlarmType::Warning, "Перегрев устройства" },                            //
        { 3, AlarmType::Warning, "Проблемы с АЦП (нет связи) (базовая)" },           //
        { 4, AlarmType::Warning, "Нет сигнала 1PPS с антенны" },                     //
        { 5, AlarmType::Warning, "Проблемы с АЦП (нет связи) (мезонин)" },           //
        { 6, AlarmType::Warning, "Ошибка регулировочных коэффициентов (базовая)" },  //
        { 7, AlarmType::Warning, "Ошибка загрузки конфигурации из flash-памяти" },   //
        { 8, AlarmType::Warning, "Некорректная Hardware информация (базовая)" },     //
        { 9, AlarmType::Warning, "Некорректная Hardware информация (мезонин)" },     //
        { 10, AlarmType::Warning, "Ошибка регулировочных коэффициентов (мезонин)" }, //
        { 11, AlarmType::Warning, "Напряжение батареи низко (< 2,5 В)" },            //
        { 12, AlarmType::Warning, "Нет связи с NTP-сервером" },                      //
        { 13, AlarmType::Warning, "Не работает внешняя flash-память (мезонин)" },    //
        { 14, AlarmType::Warning, "Не работает внешняя fram" },                      //
        { 15, AlarmType::Warning, "Проблемы с RTC" },                                //
    };
}

void AlarmStateAll::reqUpdate()
{
    engine()->currentConnection()->reqBSI();
}

void AlarmStateAll::setupUI()
{
    Q_ASSERT(m_config.size() > 0 && m_config.size() <= 32);
    auto mainLayout = new QVBoxLayout(this);
    for (std::size_t i = 0; i < m_config.size(); ++i)
    {
        Q_ASSERT(m_config[i].index < 32);
        auto hLayout = new QHBoxLayout;
        auto label = WDFunc::NewLBL2(this, "", QString::number(m_config[i].index));
        auto pixmap = WDFunc::NewCircle(m_normalColor, circleRadius);
        label->setPixmap(pixmap);
        hLayout->addWidget(label);
        hLayout->addWidget(WDFunc::NewLBL2(this, m_config[i].desc), 1);
        mainLayout->addLayout(hLayout);
    }
    mainLayout->addWidget(WDFunc::NewPB(this, "", "Ok", static_cast<QWidget *>(this), &QWidget::hide), 0);
    setLayout(mainLayout);
}

void AlarmStateAll::updateStrongestColor(const QColor &newColor)
{
    if (m_strongestColor == Qt::transparent)
    {
        m_strongestColor = newColor;
    }
    else if (m_strongestColor == Qt::green)
    {
        if (newColor != Qt::transparent)
            m_strongestColor = newColor;
    }
    else if (m_strongestColor == Qt::yellow)
    {
        if (newColor == Qt::red)
            m_strongestColor = newColor;
    }
}

void AlarmStateAll::update(const quint32 health)
{
    const std::bitset<32> healthBits { health };
    m_strongestColor = m_normalColor;
    for (auto &record : m_config)
    {
        Q_ASSERT(record.index < 32);
        bool state = healthBits.test(record.index);
        QColor color(m_normalColor);
        if (state)
            color = getColorByType(record.type);
        updateStrongestColor(color);
        auto circle = WDFunc::NewCircle(color, circleRadius);
        WDFunc::SetLBLImage(this, QString::number(record.index), &circle);
    }
    emit updateColor(m_strongestColor);
}
