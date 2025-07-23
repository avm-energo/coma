#pragma once

#include <array>
#include <oscillograms/parsemodule.h>

#define KDVOSCPOINTS 524288

class ParseID10040 : public ParseModule
{
public:
    ParseID10040(const QByteArray &BA);

    std::array<float, KDVOSCPOINTS> point;

public:
    bool Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model) override;
};
