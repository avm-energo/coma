#include "configstorage.h"

#include <interfaces/conn/baseconnection.h>
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

void ConfigStorage::protocolDescriptionReceived(const AbstractGroup &str)
{
    auto &sigMap = m_settings.getSignals();
    if (sigMap.contains(str.sigId))
    {
        auto signal = sigMap.value(str.sigId);
        ProtocolDescription *descr = Interface::BaseConnection::iface()->settings();
        switch (str.ifaceType)
        {
        case Interface::IfaceType::USB:
            descr->addGroup(ProtocomGroup { signal.startAddr, signal.count, str.arg1 });
            break;
        case Interface::IfaceType::RS485:
            descr->addGroup(ModbusGroup { signal.startAddr, signal.count, str.arg1 });
            break;
        case Interface::IfaceType::Ethernet:
            descr->addGroup(Iec104Group { signal.startAddr, signal.count, str.arg1, str.arg2 });
            break;
        default:
            break;
        }
    }
}
