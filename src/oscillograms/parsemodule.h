#ifndef PARSEMODULE_H
#define PARSEMODULE_H

#include <s2/s2datatypes.h>

class TrendViewModel;

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

#endif // PARSEMODULE_H
