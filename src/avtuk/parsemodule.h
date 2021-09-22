#ifndef PARSEMODULE_H
#define PARSEMODULE_H
#include "../gen/datatypes.h"
#include "trendviewmodel.h"

#include <memory>
class TrendViewDialog;

class ParseModule
{
public:
    ParseModule(const QByteArray &BA);
    virtual ~ParseModule();
    bool PosPlusPlus(void *dst, int &count, int size);
    virtual bool Parse(int &count) = 0;

    virtual bool Parse(quint32 id, const S2DataTypes::OscHeader &OHD, TrendViewModel *model) = 0;

    TrendViewModel *trendViewModel() const;

protected:
    const QByteArray BArray;
    std::unique_ptr<TrendViewModel> m_trendViewModel;
    TrendViewDialog *m_trendViewDialog;
    quint32 id;
};

#endif // PARSEMODULE_H
