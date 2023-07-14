#pragma once

#include "../ctti/type_id.hpp"
#include "dataitem.h"
#include "s2configstorage.h"

namespace S2
{

class DataFactory
{
private:
    const S2ConfigStorage &s2confStorage;

    ctti::unnamed_type_id_t getType(quint16 id) const;
    quint16 getId(const QString &name) const;

public:
    explicit DataFactory(const S2ConfigStorage &confStorage = S2ConfigStorage::GetInstance());

    S2::DataItem create(const S2::DataRec &record) const;
    S2::DataItem create(const quint16 id, const QByteArray &data) const;
    S2::DataItem create(quint16 id, const QString &str) const;

    template <typename T, std::enable_if_t<S2::isValueType<T>::value, bool> = true> //
    S2::DataItem create(const QString &name, const T value)
    {
        return { getId(name), value };
    }
};

}

using S2DataFactory = S2::DataFactory;
