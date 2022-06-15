#pragma once
#include "datarecv.h"

class ConfigV
{

public:
    ConfigV() = default;

    DataTypes::DataRecV getRecord(quint16 id);
    void setRecordValue(const DataTypes::DataRecV &record);

    const QList<DataTypes::DataRecV> &getConfig() const;
    void setConfig(const QList<DataTypes::DataRecV> &newConfig);

private:
    QList<DataTypes::DataRecV> config;
};
