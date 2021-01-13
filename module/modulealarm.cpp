#include "modulealarm.h"

#include "../gen/datamanager.h"

ModuleAlarm::ModuleAlarm(QWidget *parent) : BaseAlarm(parent)
{
    connect(&DataManager::GetInstance(), &DataManager::singlePointReceived, this, &ModuleAlarm::update);
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
}

// int Alarm::realAlarmSize()
//{
//    return m_realAlarmSize;
//}

/*void Alarm::Update(std::bitset<32> &states)
{
    for (int i = 0; i < m_realAlarmSize; ++i)
    {
        UpdatePixmaps(states[i], i);
    }
} */

void ModuleAlarm::update(const DataTypes::SinglePointWithTimeStruct &sp)
{
    quint32 minAddress = m_startAlarmAddress;
    quint32 maxAddress = m_startAlarmAddress + m_alarmFlags.size();
    if (!((sp.sigAdr >= minAddress) && (sp.sigAdr <= maxAddress)))
        return;
    // bool alarmFlag = false; // warning flag

    const int index = (sp.sigAdr - minAddress);
    // int index = sp.sigAdr;
    const quint8 sigval = sp.sigVal;
    if (sigval & 0x80)
        return;

    if (m_alarmFlags.test(index))
    {
        //    qDebug() << m_alarmFlags;
        //    qDebug() << index << bool(sigval);
        // alarmFlag = true;
        updatePixmap(sigval & 0x00000001, index);
    }
}
