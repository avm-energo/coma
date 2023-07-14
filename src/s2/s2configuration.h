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
};

} // namespace S2

using S2Configuration = S2::Configuration;
