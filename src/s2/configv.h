#pragma once
#include "datarecv.h"

class ConfigV
{

public:
    ConfigV() = default;

    DataTypes::DataRecV getRecord(quint16 id);
    void setRecordValue(const DataTypes::DataRecV &record);
    void setRecordValue(const quint16 key, const DataTypes::valueType &value);

    const QList<DataTypes::DataRecV> &getConfig() const;
    void setConfig(const QList<DataTypes::DataRecV> &newConfig);

private:
    QList<DataTypes::DataRecV> config;
};
