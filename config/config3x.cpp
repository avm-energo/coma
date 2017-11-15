#include "config3x.h"
#include "../publicclass.h"

// установка количества элементов в массивах конфигурационных параметров (эквивалентно количеству входов платы)

/*void Config3x::SetDInSize(Bci_DIn &Bcii, int mtype)
{
    int size = ModTypeMap().value(mtype).DInSize;
    if ((size > SIZEMAX) || !size)
        return;
    Bcii.in_type.resize(size);
    Bcii.dly_time.resize(size);
    Bcii.pair.resize(size);
}

// установка количества элементов в массивах конфигурационных параметров (эквивалентно количеству выходов платы)

void Config3x::SetDOutSize(Bci_DOut &Bcio, int mtype)
{
    int size = ModTypeMap().value(mtype).DOutSize;
    if ((size > SIZEMAX) || !size)
        return;
    Bcio.in_type.resize(size);
}

void Config3x::SetDOutRelaySize(Config3x::Bci_DOut_Relay &Bcio, int mtype)
{
    Q_UNUSED(Bcio);
    int size = ModTypeMap().value(mtype).DOutSize;
    if ((size > SIZEMAX) || !size)
        return;
}
*/
