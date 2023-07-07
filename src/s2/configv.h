#pragma once
#include "datarecv.h"

class ConfigV
{

public:
    ConfigV() = default;

    S2DataTypes::DataRecV getRecord(quint16 id);
    void setRecordValue(const S2DataTypes::DataRecV &record);
    void setRecordValue(const quint16 key, const S2DataTypes::valueType &value);

    const QList<S2DataTypes::DataRecV> &getConfig() const;
    void setConfig(const QList<S2DataTypes::DataRecV> &newConfig);

    void pushConfig();
    void popConfig();

private:
    QList<S2DataTypes::DataRecV> config;
    QList<S2DataTypes::DataRecV> savedConfig;
};
