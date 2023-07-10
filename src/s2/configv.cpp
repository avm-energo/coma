#include "configv.h"

#include <QDebug>
#include <gen/error.h>

S2DataTypes::DataItem ConfigV::getRecord(quint16 id)
{
    auto result = std::find_if(config.cbegin(), config.cend(), //
        [id](const auto &record) { return (id == record.getId()); });
    if (result != config.cend())
        return *result;
    else
    {
        qDebug() << Error::NullDataError << id;
        return S2DataTypes::DataItem(id);
    }
}

void ConfigV::setRecordValue(const S2DataTypes::DataItem &record)
{
    auto result = std::find_if(config.begin(), config.end(), //
        [&record](const auto &lhs) { return (lhs.getId() == record.getId()); });
    if (result != config.end())
    {
        // buffer is here for debug purposes
        [[maybe_unused]] S2DataTypes::DataItem &buffer = *result;
        Q_ASSERT(result->typeIndex() == record.typeIndex());
        *result = record;
    }
    else
        config.push_back(record);
}

void ConfigV::setRecordValue(const quint16 key, const S2DataTypes::valueType &value)
{
    auto result
        = std::find_if(std::begin(config), std::end(config), [key](const auto &lhs) { return (lhs.getId() == key); });
    if (result != std::end(config))
    {
        result->setData(value);
    }
    else
    {
        S2DataTypes::DataItem record(key);
        record.setData(value);
        config.push_back(record);
    }
}

const QList<S2DataTypes::DataItem> &ConfigV::getConfig() const
{
    return config;
}

void ConfigV::setConfig(const QList<S2DataTypes::DataItem> &newConfig)
{
    config = newConfig;
}

void ConfigV::pushConfig()
{
    savedConfig = config;
}

void ConfigV::popConfig()
{
    config = savedConfig;
}
