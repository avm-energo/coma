#pragma once

#include <oscillograms/parsers/parsemodule.h>

class ParseID10001 : public ParseModule
{
public:
    ParseID10001(const QByteArray &BA);

    struct Point21
    {
        float An;
    };

public:
    bool Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model) override;
};
