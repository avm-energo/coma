#include "configstorage.h"

#include <interfaces/connection.h>
#include <interfaces/types/interfacesettings.h>

ConfigStorage::ConfigStorage(token, QObject *parent) : QObject(parent)
{
}

const ModuleSettings &ConfigStorage::getModuleSettings() const
{
    return m_settings;
}

void ConfigStorage::clearModuleSettings()
{
    m_settings.clear();
}

void ConfigStorage::signalDataReceive(const quint32 id, const quint32 addr, //
    const quint16 count, const ModuleTypes::SignalType sigType)
{
    if (id == 0)
        qWarning() << "Invalid signal id: " << id;
    else if (addr == 0)
        qWarning() << "Invalid signal address, signal id: " << id;
    else if (count == 0)
        qWarning() << "Invalid signal count, signal id: " << id;
    else
        m_settings.appendSignal(id, { addr, count, sigType });
}

void ConfigStorage::tabDataReceive(const quint32 id, const QString &name)
{
    if (id == 0)
        qWarning() << "Invalid tab id: " << id;
    else if (name == "")
        qWarning() << "Empty tab name, tab id: " << id;
    else
        m_settings.appendTab(id, name);
}

void ConfigStorage::sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead)
{
    m_settings.appendSection({ secHead, sgmap });
}

void ConfigStorage::alarmDataReceive(const Modules::AlarmType aType, const quint32 addr, //
    const QString &desc, const QList<quint32> &highlights)
{
    m_settings.appendAlarm(aType, addr, desc);
    m_settings.appendHighlight(aType, addr, highlights);
}

void ConfigStorage::workJourDataReceive(const quint32 id, const QString &desc)
{
    m_settings.appendWorkJournal(id, desc);
}

void ConfigStorage::measJourDataReceive(const quint32 index, const QString &header, //
    const ModuleTypes::BinaryType type, bool visib)
{
    m_settings.appendMeasJournal(index, header, type, visib);
}

void ConfigStorage::hiddenTabDataReceived(const ModuleTypes::HiddenTab &hiddenTab)
{
    m_settings.appendHiddenTab(hiddenTab);
}

void ConfigStorage::protocolDescriptionReceived(const AbstractGroup &str)
{
    using namespace Interface;
    auto &sigMap = m_settings.getSignals();
    if (sigMap.find(str.sigId) != sigMap.cend())
    {
        auto signal = sigMap.at(str.sigId);
        // ProtocolDescription *descr = Connection::iface()->settings();
        ProtocolDescription description;
        switch (str.ifaceType)
        {
        case Interface::IfaceType::USB:
            description.addGroup(ProtocomGroup { signal.startAddr, signal.count, str.arg1 });
            break;
        case Interface::IfaceType::RS485:
            description.addGroup(ModbusGroup { signal.startAddr, signal.count, str.arg1 });
            break;
        case Interface::IfaceType::Ethernet:
            description.addGroup(Iec104Group { signal.startAddr, signal.count, str.arg1, str.arg2 });
            break;
        default:
            break;
        }
    }
}
