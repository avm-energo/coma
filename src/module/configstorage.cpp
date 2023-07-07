#include "configstorage.h"

#include "../interfaces/baseinterface.h"
#include "../interfaces/interfacesettings.h"

ConfigStorage::ConfigStorage(token, QObject *parent) : QObject(parent), mSettings(new ModuleSettings), isS2Parsed(false)
{
}

/// \brief Constant getter for S2 parsing status.
bool ConfigStorage::getS2Status() const
{
    return isS2Parsed;
}

/// \brief Setter for S2 parsing status.
void ConfigStorage::setS2Status(const bool &status)
{
    isS2Parsed = status;
}

/// \brief Constant getter for S2 configs hashmap.
const TypeByIdMap &ConfigStorage::getS2Map() const
{
    return s2Map;
}

/// \brief Constant getter for S2 widget hashmap.
const config::widgetMap &ConfigStorage::getWidgetMap() const
{
    return widgetMap;
}

/// \brief Constant getter for S2 tabs map.
const ModuleTypes::TabsMap &ConfigStorage::getConfigTabs() const
{
    return s2tabs;
}

/// \brief Constant getter for module settings.
const ModuleSettings &ConfigStorage::getModuleSettings() const
{
    return *mSettings;
}

/// \brief Cleaning module settings.
void ConfigStorage::clearModuleSettings()
{
    mSettings->clear();
}

/// \brief Slot for saving S2 config record.
void ConfigStorage::typeDataReceive(const quint16 id, const uint64_t typeId)
{
    if (id == 0)
        qWarning() << "Invalid S2 config id: " << id;
    else if (typeId != 0)
        s2Map.insert({ id, typeId });
}

/// \brief Slot for saving S2 widget record.
void ConfigStorage::widgetDataReceive(const quint16 id, const config::itemVariant &widget)
{
    if (id == 0)
        qWarning() << "Invalid S2 widget id: " << id;
    else if (widget.valueless_by_exception())
        qWarning() << "Invalid S2 widget data, widget id: " << id;
    else
        widgetMap.insert({ id, widget });
}

/// \brief Slot for saving S2 tab record.
void ConfigStorage::configTabDataReceive(const quint32 &id, const QString &tabName)
{
    if (tabName == "")
        qWarning() << "Empty tab name, tab id: " << id;
    else
        s2tabs.insert(id, tabName);
}

/// \brief Slot for starting new config in module settings (for second configuration file).
void ConfigStorage::startNewConfig()
{
    mSettings->startNewConfig();
}

/// \brief Slot for saving module's signal record.
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
        mSettings->appendSignal(id, { addr, count, sigType });
}

/// \brief Slot for saving check's tab record.
void ConfigStorage::tabDataReceive(const quint32 id, const QString &name)
{
    if (id == 0)
        qWarning() << "Invalid tab id: " << id;
    else if (name == "")
        qWarning() << "Empty tab name, tab id: " << id;
    else
        mSettings->appendTab(id, name);
}

/// \brief Slot for saving check's section record.
void ConfigStorage::sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead)
{
    mSettings->appendSection({ secHead, sgmap });
}

/// \brief Slot for saving module's alarm record.
void ConfigStorage::alarmDataReceive(const Modules::AlarmType aType, const quint32 addr, //
    const QString &desc, const QList<quint32> &highlights)
{
    mSettings->appendAlarm(aType, addr, desc);
    mSettings->appendHighlight(aType, addr, highlights);
}

/// \brief Slot for saving module a work journal's record.
void ConfigStorage::workJourDataReceive(const quint32 id, const QString &desc)
{
    mSettings->appendWorkJournal(id, desc);
}

/// \brief Slot for saving module a measurement journal's record.
void ConfigStorage::measJourDataReceive(const quint32 index, const QString &header, //
    const ModuleTypes::BinaryType type, bool visib)
{
    mSettings->appendMeasJournal(index, header, type, visib);
}

/// \brief Slot for saving module's config record.
void ConfigStorage::configDataReceive(const quint16 id, const QString &defVal, const bool visib, const quint16 count)
{
    if (id == 0)
        qWarning() << "Invalid config id: " << id;
    else if (defVal == "")
        qWarning() << "Invalid default value, config id: " << id;
    else
    {
        mSettings->appendToCurrentConfig({ { id, defVal }, visib });
        if (count != 0)
            mSettings->appendDetailCount(id, count);
    }
}

/// \brief Slot for saving module's protocol groups
void ConfigStorage::protocolDescriptionReceived(const parseXChangeStruct &str)
{
    auto &sigMap = getModuleSettings().getSignals();
    if (sigMap.contains(str.sigId))
    {
        auto signal = sigMap.value(str.sigId);
        Board::InterfaceType ifaceType = str.interfaceType.value<Board::InterfaceType>();
        ProtocolDescription *descr = Interface::BaseInterface::iface()->settings();
        switch (ifaceType)
        {
        case Board::USB:
            descr->addGroup(ProtocomGroup({ signal.startAddr, signal.count, str.par2.value<quint16>() }));
            break;
        case Board::RS485:
            descr->addGroup(ModbusGroup({ signal.startAddr, signal.count, str.par2.value<quint16>() }));
            break;
        case Board::Ethernet:
            descr->addGroup(
                Iec104Group({ signal.startAddr, signal.count, str.par2.value<quint16>(), str.par3.value<quint16>() }));
            break;
        default:
            break;
        }
    }
}
