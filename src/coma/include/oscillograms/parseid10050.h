#pragma once

#include <array>
#include <oscillograms/parsemodule.h>

#define KDVSPECPOINTS 524288

class ParseID10050 : public ParseModule
{
public:
    ParseID10050(const QByteArray &BA);

    std::array<float, KDVSPECPOINTS> point;

public:
    bool Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model) override;
};
