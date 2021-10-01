#include "configv.h"

#include "error.h"

#include <QDebug>

DataTypes::DataRecV ConfigV::getRecord(unsigned int id)
{
    auto result = std::find_if(
        std::cbegin(config), std::cend(config), [id](const auto &record) { return (id == record.getId()); });
    if (result != std::cend(config))
        return *result;
    else
    {
        qDebug() << Error::NullDataError << id;
        return DataTypes::DataRecV(id);
    }
}

void ConfigV::setRecordValue(const DataTypes::DataRecV &record)
{

    auto result = std::find_if(
        std::begin(config), std::end(config), [record](const auto &lhs) { return (lhs.getId() == record.getId()); });
    if (result != std::end(config))
    {
        // buffer is here for debug purposes
        [[maybe_unused]] DataTypes::DataRecV &buffer = *result;
        Q_ASSERT(result->typeIndex() == record.typeIndex());
        *result = record;
    }
    else
        config.push_back(record);
}

const QList<DataTypes::DataRecV> &ConfigV::getConfig() const
{
    return config;
}

void ConfigV::setConfig(const QList<DataTypes::DataRecV> &newConfig)
{
    config = newConfig;
}
