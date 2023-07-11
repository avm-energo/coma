#pragma once

#include "../ctti/type_id.hpp"
#include "dataitem.h"

namespace S2
{

class ConfigStorage;

class DataFactory
{
private:
    const ConfigStorage &confStorage;

    ctti::unnamed_type_id_t getType(quint16 id) const;
    quint16 getId(const QString &name) const;

public:
    explicit DataFactory();

    S2DataTypes::DataItem create(const S2DataTypes::DataRec &record) const;
    S2DataTypes::DataItem create(const quint16 id, const QByteArray &data) const;
    S2DataTypes::DataItem create(quint16 id, const QString &str) const;

    template <typename T, std::enable_if_t<S2DataTypes::isValueType<T>::value, bool> = true> //
    S2DataTypes::DataItem create(const QString &name, const T value)
    {
        return { getId(name), value };
    }
};

}
