#include "device/configstorage.h"

namespace Device
{

ConfigStorage::ConfigStorage(QObject *parent) : QObject(parent)
{
}

const XmlSettings &ConfigStorage::getDeviceSettings() const noexcept
{
    return m_settings;
}

const ProtocolDescription &ConfigStorage::getProtocolDescription() const noexcept
{
    return m_protocol;
}

void ConfigStorage::clear() noexcept
{
    m_settings.clear();
    m_protocol.clear();
}

void ConfigStorage::signalDataReceive(const u32 id, const u32 addr, //
    const u16 count, const XmlDataTypes::SignalType type)
{
    if (id == 0)
        qWarning() << "Invalid signal id: " << id;
    else if (addr == 0)
        qWarning() << "Invalid signal address, signal id: " << id;
    else if (count == 0)
        qWarning() << "Invalid signal count, signal id: " << id;
    else
        m_settings.appendSignal(id, { addr, count, type });
}

void ConfigStorage::tabDataReceive(const u32 id, const QString &name)
{
    if (id == 0)
        qWarning() << "Invalid tab id: " << id;
    else if (name == "")
        qWarning() << "Empty tab name, tab id: " << id;
    else
        m_settings.appendTab(id, name);
}

void ConfigStorage::sectionDataReceive(const XmlDataTypes::SGMap &sgmap, const QString &secHead)
{
    m_settings.appendSection({ secHead, sgmap });
}

void ConfigStorage::alarmDataReceive(const XmlDataTypes::AlarmType type, const u32 addr, //
    const QString &desc, const QList<u32> &highlights)
{
    m_settings.appendAlarm(type, addr, desc);
    m_settings.appendHighlight(type, addr, highlights);
}

void ConfigStorage::workJourDataReceive(const u32 id, const QString &desc)
{
    m_settings.appendWorkJournal(id, desc);
}

void ConfigStorage::measJourDataReceive(const u32 index, const QString &header, //
    const XmlDataTypes::BinaryType type, bool visib)
{
    m_settings.appendMeasJournal(index, header, type, visib);
}

void ConfigStorage::hiddenTabDataReceived(const XmlDataTypes::HiddenTab &hiddenTab)
{
    m_settings.appendHiddenTab(hiddenTab);
}

void ConfigStorage::protocolGroupReceived(const Protocol::AbstractGroup &group)
{
    using namespace Interface;
    auto &sigMap = m_settings.getSignals();
    auto search = sigMap.find(group.signalId);
    if (search != sigMap.cend())
    {
        auto signal = search->second;
        switch (group.ifaceType)
        {
        case Interface::IfaceType::USB:
            m_protocol.addGroup(Protocol::ProtocomGroup { signal.startAddr, signal.count, group.arg1 });
            break;
        case Interface::IfaceType::RS485:
            m_protocol.addGroup(Protocol::ModbusGroup { signal.startAddr, signal.count, group.arg1 });
            break;
        case Interface::IfaceType::Ethernet:
            m_protocol.addGroup(Protocol::Iec104Group { signal.startAddr, signal.count, group.arg1, group.arg2 });
            break;
        default:
            break;
        }
    }
}

} // namespace Device
