#include "configv.h"

#include <QDebug>
#include <gen/error.h>

S2::DataItem ConfigV::getRecord(quint16 id)
{
    auto result = std::find_if(config.cbegin(), config.cend(), //
        [id](const auto &record) { return (id == record.getId()); });
    if (result != config.cend())
        return *result;
    else
    {
        qDebug() << Error::NullDataError << id;
        return S2::DataItem(id);
    }
}

void ConfigV::setRecordValue(const S2::DataItem &record)
{
    auto result = std::find_if(config.begin(), config.end(), //
        [&record](const auto &lhs) { return (lhs.getId() == record.getId()); });
    if (result != config.end())
    {
        // buffer is here for debug purposes
        [[maybe_unused]] S2::DataItem &buffer = *result;
        Q_ASSERT(result->typeIndex() == record.typeIndex());
        *result = record;
    }
    else
        config.push_back(record);
}

void ConfigV::setRecordValue(const quint16 key, const S2::valueType &value)
{
    auto result
        = std::find_if(std::begin(config), std::end(config), [key](const auto &lhs) { return (lhs.getId() == key); });
    if (result != std::end(config))
    {
        result->setData(value);
    }
    else
    {
        S2::DataItem record(key);
        record.setData(value);
        config.push_back(record);
    }
}

const QList<S2::DataItem> &ConfigV::getConfig() const
{
    return config;
}

void ConfigV::setConfig(const QList<S2::DataItem> &newConfig)
{
    config = newConfig;
}

ConfigV ConfigV::copy() const
{
    ConfigV copy;
    QList<S2::DataItem> configCopy;
    std::copy(config.cbegin(), config.cend(), configCopy.begin());
    copy.setConfig(configCopy);
    return copy;
}

// void ConfigV::pushConfig()
//{
//    savedConfig = config;
//}

// void ConfigV::popConfig()
//{
//    config = savedConfig;
//}
