#pragma once

#include "parsemodule.h"

#include <array>
#define KDVSPECPOINTS 524288

class ParseID10050 : public ParseModule
{
public:
    ParseID10050(const QByteArray &BA);

    //    struct PointSpec
    //    {
    //        std::array<float, 6> An;
    //    };
    std::array<float, KDVSPECPOINTS> point;

public:
    bool Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model) override;
};
