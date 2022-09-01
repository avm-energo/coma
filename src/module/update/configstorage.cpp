#include "configstorage.h"

ConfigStorage::ConfigStorage(token, QObject *parent) : QObject(parent), mSettings(new ModuleSettings), isS2Parsed(false)
{
}

const bool ConfigStorage::getS2Status() const
{
    return isS2Parsed;
}

void ConfigStorage::setS2Status(const bool status)
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

void ConfigStorage::typeDataReceive(const quint16 &id, const ctti::unnamed_type_id_t &type)
{
    if (id != 0 && type != 0)
        mS2Map.insert(id, type);
}

void ConfigStorage::widgetDataReceive(const quint16 &id, const config::itemVariant &widget)
{
    if (id != 0)
        mWidgetMap.insert({ id, widget });
}
