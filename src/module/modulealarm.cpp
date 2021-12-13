#include "modulealarm.h"

#include "../gen/datamanager.h"

ModuleAlarm::ModuleAlarm(QWidget *parent) : BaseAlarm(parent)
{
    connect(&DataManager::GetInstance(), &DataManager::singlePointReceived, this,
        qOverload<const DataTypes::SinglePointWithTimeStruct &>(&ModuleAlarm::update));
}

ModuleAlarm::ModuleAlarm(const DataTypes::Alarm &desc, const int count, QWidget *parent) : ModuleAlarm(parent)
{
    m_startAlarmAddress = desc.startAddr;
    m_alarmFlags = std::bitset<sizeof(desc.flags) * 8>(desc.flags);
    Q_ASSERT(QColor::isValidColor(desc.color));
    m_alarmColor = desc.color;
    m_alarmAllCounts = count;
    setupUI(desc.desc);
}

void ModuleAlarm::reqUpdate()
{
    BaseInterface::iface()->reqAlarms(m_startAlarmAddress, m_alarmAllCounts);
    update();
}

void ModuleAlarm::update(const DataTypes::SinglePointWithTimeStruct &sp)
{
    const auto minAddress = m_startAlarmAddress;
    const auto maxAddress = m_startAlarmAddress + m_alarmFlags.size();
    if (!((sp.sigAdr >= minAddress) && (sp.sigAdr <= maxAddress)))
        return;

    const int index = (sp.sigAdr - minAddress);

    const quint8 sigval = sp.sigVal;
    if (sigval & 0x80)
        return;

    if (m_alarmFlags.test(index))
    {
        updatePixmap(sigval & 0x00000001, index);
    }
}

void ModuleAlarm::update()
{
    using ValueType = DataTypes::SinglePointWithTimeStruct;
    const auto minAddress = m_startAlarmAddress;
    const auto maxAddress = m_startAlarmAddress + m_alarmFlags.size();
    auto &manager = DataManager::GetInstance();
    for (auto i = minAddress; i != maxAddress; ++i)
    {
        if (!manager.containsRegister<ValueType>(i))
            continue;
        const auto sp = manager.getRegister<ValueType>(i);
        const int index = (sp.sigAdr - minAddress);
        const quint8 sigval = sp.sigVal;
        if (sigval & 0x80)
            return;

        if (m_alarmFlags.test(index))
        {
            updatePixmap(sigval & 0x00000001, index);
        }
    }
}
