#pragma once
#include "dataitem.h"

class ConfigV
{

public:
    ConfigV() = default;

    S2DataTypes::DataItem getRecord(quint16 id);
    void setRecordValue(const S2DataTypes::DataItem &record);
    void setRecordValue(const quint16 key, const S2DataTypes::valueType &value);

    const QList<S2DataTypes::DataItem> &getConfig() const;
    void setConfig(const QList<S2DataTypes::DataItem> &newConfig);

    void pushConfig();
    void popConfig();

private:
    QList<S2DataTypes::DataItem> config;
    QList<S2DataTypes::DataItem> savedConfig;
};
