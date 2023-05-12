#include "s2datatypes.h"

#include <QDebug>
#include <gen/timefunc.h>

QDebug S2DataTypes::operator<<(QDebug debug, const S2DataTypes::OscInfo &st)
{
    debug.nospace() << st.typeHeader.id << ":" << st.typeHeader.numByte << ":" << st.id << ":"
                    << TimeFunc::UnixTime64ToString(st.unixtime) << ":" << st.idOsc0;
    return debug.maybeSpace();
}
