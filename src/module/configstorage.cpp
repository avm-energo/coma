#include "configstorage.h"

#include "../interfaces/baseinterface.h"
#include "../interfaces/interfacesettings.h"

ConfigStorage::ConfigStorage(token, QObject *parent) : QObject(parent), mSettings(new ModuleSettings), isS2Parsed(false)
{
}

/// \brief Constant getter for S2 parsing status.
const bool &ConfigStorage::getS2Status() const
{
    return isS2Parsed;
}

/// \brief Setter for S2 parsing status.
void ConfigStorage::setS2Status(const bool &status)
{
    isS2Parsed = status;
}

/// \brief Constant getter for S2 configs hashmap.
const DataTypes::ValueMap::value_type &ConfigStorage::getS2Map() const
{
    return mS2Map.map();
}

/// \brief Constant getter for S2 widget hashmap.
const config::widgetMap &ConfigStorage::getWidgetMap() const
{
    return mWidgetMap;
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
void ConfigStorage::typeDataReceive(const quint16 &id, const uint64_t &typeId)
{
    if (id != 0 && typeId != 0)
        mS2Map.insert(id, typeId);
}

/// \brief Slot for saving S2 widget record.
void ConfigStorage::widgetDataReceive(const quint16 &id, const config::itemVariant &widget)
{
    if (id != 0 && !widget.valueless_by_exception())
        mWidgetMap.insert({ id, widget });
}

/// \brief Slot for saving S2 tab record.
void ConfigStorage::configTabDataReceive(const quint32 &id, const QString &tabName)
{
    if (tabName != "")
        s2tabs.insert(id, tabName);
}

/// \brief Slot for starting new config in module settings (for second configuration file).
void ConfigStorage::startNewConfig()
{
    mSettings->startNewConfig();
}

/// \brief Slot for saving module's signal record.
void ConfigStorage::signalDataReceive(const quint32 &id, const quint32 &addr, //
    const quint16 &count, const ModuleTypes::SignalType &sigType)
{
    if (id != 0 && addr != 0 && count != 0)
        mSettings->appendSignal(id, { addr, count, sigType });
}

/// \brief Slot for saving check's tab record.
void ConfigStorage::tabDataReceive(const quint32 &id, const QString &name)
{
    if (id != 0 and name != "")
        mSettings->appendTab(id, name);
}

/// \brief Slot for saving check's section record.
void ConfigStorage::sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead)
{
    mSettings->appendSection({ secHead, sgmap });
}

/// \brief Slot for saving module's alarm record.
void ConfigStorage::alarmDataReceive(const Modules::AlarmType &aType, const quint32 &addr, const QString &desc)
{
    mSettings->appendAlarm(aType, addr, desc);
}

/// \brief Slot for saving module's journal record.
void ConfigStorage::jourDataReceive(const Modules::JournalType &jType, const quint32 &addr, const QString &desc)
{
    mSettings->appendJournal(jType, { addr, desc });
}

/// \brief Slot for saving module's interface settings.
void ConfigStorage::interfaceSettingsReceive(const QVariant &iSettings, const Board::InterfaceType &iType)
{
    if (iSettings.isValid())
    {
        // A L E R T! Говнокод, но по-другому сделать с двумя файлами будет сложнее :/
        auto ifSettings = mSettings->getInterfaceSettings().settings;
        if (!ifSettings.isNull() && ifSettings.isValid())
        {
            if (iType == Board::USB || iType == Board::Emulator)
            {
                auto ifOld = ifSettings.value<InterfaceInfo<ProtocomGroup>>();
                auto ifNew = iSettings.value<InterfaceInfo<ProtocomGroup>>();
                for (const auto &val : ifNew.dictionary())
                {
                    ifOld.addGroup(val);
                }
                ifSettings.setValue(ifOld);
            }
            else if (iType == Board::RS485)
            {
                auto ifOld = ifSettings.value<InterfaceInfo<ModbusGroup>>();
                auto ifNew = iSettings.value<InterfaceInfo<ModbusGroup>>();
                for (const auto &val : ifNew.dictionary())
                {
                    ifOld.addGroup(val);
                }
                ifSettings.setValue(ifOld);
            }
            else if (iType == Board::Ethernet)
            {
                auto ifOld = ifSettings.value<InterfaceInfo<Iec104Group>>();
                auto ifNew = iSettings.value<InterfaceInfo<Iec104Group>>();
                for (const auto &val : ifNew.dictionary())
                {
                    ifOld.addGroup(val);
                }
                ifSettings.setValue(ifOld);
            }
            mSettings->setInterfaceSettings({ ifSettings });
            BaseInterface::iface()->setSettings(mSettings->getInterfaceSettings());
        }
        else
        {
            mSettings->setInterfaceSettings({ iSettings });
            BaseInterface::iface()->setSettings(mSettings->getInterfaceSettings());
        }
    }
}

/// \brief Slot for saving module's config record.
void ConfigStorage::configDataReceive(const quint32 &id, //
    const QString &defVal, const bool &visib, const quint32 &count)
{
    if (id != 0 && defVal != "")
    {
        mSettings->appendToCurrentConfig({ { id, defVal }, visib });
        // TODO: парсить это дело в новую мапу: key = quint32 (id), value = quint32 (count).
        Q_UNUSED(count);
    }
}
