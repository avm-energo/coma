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

    ctti::unnamed_type_id_t getType(quint32 id) const;

public:
    explicit DataFactory(const S2ConfigStorage &confStorage = S2ConfigStorage::GetInstance());

    DataItem create(const DataRec &record) const;
    DataItem create(const quint32 id, const QByteArray &data) const;
    DataItem create(const quint32 id, const QString &str) const;

    template <typename T, std::enable_if_t<isValueType<T>::value, bool> = true> //
    DataItem create(const quint32 id, const T &value)
    {
        return { id, value };
    }
};

}

using S2DataFactory = S2::DataFactory;
