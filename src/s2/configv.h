#pragma once
#include "dataitem.h"

class ConfigV
{

public:
    ConfigV() = default;

    S2::DataItem getRecord(quint16 id);
    void setRecordValue(const S2::DataItem &record);
    void setRecordValue(const quint16 key, const S2::valueType &value);

    const QList<S2::DataItem> &getConfig() const;
    void setConfig(const QList<S2::DataItem> &newConfig);
    ConfigV copy() const;

private:
    QList<S2::DataItem> config;
};
