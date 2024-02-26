#include "alarms/alarmstateall.h"

#include "../../widgets/wd_func.h"

#include <QBoxLayout>
#include <bitset>
#include <device/current_device.h>

AlarmStateAll::AlarmStateAll(Device::CurrentDevice *device, QWidget *parent)
    : BaseAlarm(device, parent)
    , m_config(device->getConfigStorage()->getDeviceSettings().getStateAllConfig())
    , m_strongestColor(Qt::transparent)
{
    Q_ASSERT(m_config.size() <= 32);
    setupUI();
}

void AlarmStateAll::reqUpdate()
{
    engine()->currentConnection()->reqBSI();
}

void AlarmStateAll::setupUI()
{
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
    m_strongestColor = Qt::transparent;
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
