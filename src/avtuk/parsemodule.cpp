#include "parsemodule.h"

#include "trendviewdialog.h"
ParseModule::ParseModule(QByteArray &BA)
{
    BArray = BA;
}

bool ParseModule::PosPlusPlus(void *dst, int &count, int size)
{
    if (count > BArray.size())
        return false;
    memcpy(dst, &(BArray.data()[count]), size);
    count = count + size;

    return true;
}

TrendViewModel *ParseModule::trendViewModel() const
{
    return m_trendViewModel.get();
}
