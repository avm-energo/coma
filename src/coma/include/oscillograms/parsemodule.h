#pragma once

#include <oscillograms/trendview/trendviewmodel.h>
#include <s2/s2datatypes.h>

class ParseModule
{
public:
    ParseModule(const QByteArray &BA);
    virtual ~ParseModule();
    bool PosPlusPlus(void *dst, int &count, int size);

    virtual bool Parse(quint32 id, const S2::OscHeader &OHD, TrendViewModel *model) = 0;

private:
    const QByteArray BArray;
};
