#include "configstorage.h"

ConfigStorage::ConfigStorage(QObject *parent) : QObject(parent), mSettings(nullptr), isS2Parsed(false)
{
}

void ConfigStorage::initSettings(const Modules::StartupInfoBlock &startupInfo)
{
    mSettings = std::unique_ptr<ModuleSettings>(new ModuleSettings(startupInfo));
}

const bool ConfigStorage::getS2Status() const
{
    return isS2Parsed;
}

void ConfigStorage::setS2Status(const bool status)
{
    isS2Parsed = status;
}

void ConfigStorage::typeDataReceive(const quint16 &id, const ctti::unnamed_type_id_t &type)
{
    mS2Map.insert(id, type);
}

void ConfigStorage::widgetDataReceive(const quint16 &id, const config::itemVariant &widget)
{
    mWidgetMap.insert({ id, widget });
}
