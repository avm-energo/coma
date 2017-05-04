#include "config2x.h"

// установка количества элементов в массивах конфигурационных параметров (эквивалентно количеству входов платы)

void Config2x::SetAInSize(Bci_AIn &Bcii, int mtype)
{
    int size = ModTypeMap().value(mtype).AInSize;
    if ((size > SIZEMAX) || !size)
        return;
    Bcii.in_max.resize(size);
    Bcii.in_min.resize(size);
    Bcii.in_type.resize(size);
    Bcii.in_vmax.resize(size);
    Bcii.in_vmin.resize(size);
    Bcii.setmax.resize(size);
    Bcii.setmaxmax.resize(size);
    Bcii.setmin.resize(size);
    Bcii.setminmin.resize(size);
    Bcii.osc.resize(size);
}

// установка количества элементов в массивах конфигурационных параметров (эквивалентно количеству выходов платы)

void Config2x::SetAOutSize(Bci_AOut &Bcio, int mtype)
{
    int size = ModTypeMap().value(mtype).AOutSize;
    if ((size > SIZEMAX) || !size)
        return;
    Bcio.out_max.resize(size);
    Bcio.out_min.resize(size);
    Bcio.out_type.resize(size);
    Bcio.out_vmax.resize(size);
    Bcio.out_vmin.resize(size);
}
