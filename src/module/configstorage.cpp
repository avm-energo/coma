#include "configstorage.h"

#include "../interfaces/baseinterface.h"

ConfigStorage::ConfigStorage(token, QObject *parent) : QObject(parent), mSettings(new ModuleSettings), isS2Parsed(false)
{
}

const bool &ConfigStorage::getS2Status() const
{
    return isS2Parsed;
}

void ConfigStorage::setS2Status(const bool &status)
{
    isS2Parsed = status;
}

const DataTypes::ValueMap::value_type &ConfigStorage::getS2Map() const
{
    return mS2Map.map();
}

const config::widgetMap &ConfigStorage::getWidgetMap() const
{
    return mWidgetMap;
}

const ModuleSettings &ConfigStorage::getModuleSettings() const
{
    return *mSettings;
}

void ConfigStorage::typeDataReceive(const quint16 &id, const uint64_t &typeId)
{
    if (id != 0 && typeId != 0)
        mS2Map.insert(id, typeId);
}

void ConfigStorage::widgetDataReceive(const quint16 &id, const config::itemVariant &widget)
{
    if (id != 0 && !widget.valueless_by_exception())
        mWidgetMap.insert({ id, widget });
}

////////////////////////////////////////

void ConfigStorage::startNewConfig()
{
    mSettings->startNewConfig();
}

void ConfigStorage::signalDataReceive(const quint32 &id, const quint32 &addr, //
    const quint16 &count, const ModuleTypes::SignalType &sigType)
{
    if (id != 0 && addr != 0 && count != 0)
    {
        auto &sigs = mSettings->getSignals();
        sigs.insert(id, { addr, count, sigType });
    }
}

void ConfigStorage::tabDataReceive(const quint32 &id, const QString &name)
{
    if (id != 0 and name != "")
    {
        auto &tabs = mSettings->getTabs();
        tabs.insert(id, name);
    }
}

void ConfigStorage::sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead)
{
    auto &sectMap = mSettings->getSections();
    sectMap.push_back({ secHead, sgmap });
}

void ConfigStorage::alarmDataReceive(const bool &isBase, //
    const Modules::AlarmType &aType, const quint32 &addr, const QString &desc)
{
    auto &alarms = mSettings->getAlarms();
    ModuleTypes::AlarmKey key(isBase, aType);
    alarms[key].insert(addr, desc);
}

void ConfigStorage::jourDataReceive(const Modules::JournalType &jType, const quint32 &addr, const QString &desc)
{
    auto &jours = mSettings->getJours();
    jours[jType].push_back({ addr, desc });
}

void ConfigStorage::interfaceSettingsReceive(const QVariant &iSettings)
{
    if (iSettings.isValid())
    {
        mSettings->setInterfaceSettings({ iSettings });
        BaseInterface::iface()->setSettings(mSettings->getInterfaceSettings());
    }
}

void ConfigStorage::configDataReceive(const quint32 &id, //
    const QString &defVal, const bool &visib, const quint32 &count)
{
    if (id != 0 && defVal != "")
    {
        auto &configs = mSettings->getConfigs();
        configs.push_back({ { id, defVal }, visib });
        // TODO: парсить это дело в новую мапу: key = quint32 (id), value = quint32 (count).
        Q_UNUSED(count);
    }
}
