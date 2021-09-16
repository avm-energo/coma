#ifndef PARSEMODULE_H
#define PARSEMODULE_H

#include "trendviewmodel.h"

class TrendViewDialog;

class ParseModule
{
public:
    ParseModule(QByteArray &BA);

    bool PosPlusPlus(void *dst, int &count, int size);
    virtual bool Parse(int &count) = 0;

    TrendViewModel *trendViewModel() const;

protected:
    QByteArray BArray;
    std::unique_ptr<TrendViewModel> m_trendViewModel;
    TrendViewDialog *m_trendViewDialog;
    quint32 id;
};

#endif // PARSEMODULE_H
