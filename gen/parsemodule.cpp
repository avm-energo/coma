#include "parsemodule.h"

ParseModule::ParseModule(QByteArray &BA)
{
    BArray = BA;
}

/*int*/void ParseModule::PosPlusPlus(void *dst, int &count, int size)
{
    memcpy(dst, &(BArray.data()[count]), size);
    count = count + size;
    //return true;
}
