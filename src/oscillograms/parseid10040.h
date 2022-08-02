#pragma once

#include "parsemodule.h"

#include <array>
#define KDVOSCPOINTS 524288

class ParseID10040 : public ParseModule
{
public:
    ParseID10040(const QByteArray &BA);

    std::array<float, KDVOSCPOINTS> point;
    //    struct PointKDV
    //    {
    //        std::array<float, KDVOSCPOINTS> An;
    //    };

public:
    bool Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model) override;
};
