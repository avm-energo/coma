#include "parsemodule.h"

ParseModule::ParseModule(QByteArray &BA)
{
    BArray = BA;
}

int ParseModule::PosPlusPlus(void *dst, int &count, int size)
{
    if (count > BArray.size())
        return false;
    memcpy(dst, &(BArray.data()[count]), size);
    count = count + size;

    return true;
}
