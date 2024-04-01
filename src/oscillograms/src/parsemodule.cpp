#include "oscillograms/parsemodule.h"

ParseModule::ParseModule(const QByteArray &BA) : BArray(BA)
{
}

ParseModule::~ParseModule()
{
}

bool ParseModule::PosPlusPlus(void *dst, int &count, int size)
{
    if (count > BArray.size())
        return false;
    memcpy(dst, &(BArray.data()[count]), size);
    count = count + size;

    return true;
}
