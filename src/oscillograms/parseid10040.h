#pragma once

#include "parsemodule.h"

#include <array>

class ParseID10040 : public ParseModule
{
public:
    ParseID10040(const QByteArray &BA);

    struct PointKDV
    {
        std::array<float, 6> An;
        float res;
    };

public:
    bool Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model) override;
};
