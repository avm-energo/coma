#pragma once

#include "s2datafactory.h"

namespace S2
{

class Configuration
{
private:
    S2DataFactory factory;
    std::map<quint16, DataItem> data;

public:
    Configuration(const S2ConfigStorage &storage);

    bool append(const S2::DataRec &record);
    bool append(const quint16 id, const QByteArray &bytes);
    bool append(const quint16 id, const QString &str);

    template <typename T> //
    bool append(const QString &name, const T &value)
    {
        ;
    }
};

} // namespace S2

using S2Configuration = S2::Configuration;
