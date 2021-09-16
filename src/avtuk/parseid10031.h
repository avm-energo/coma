#ifndef PARSEID10031_H
#define PARSEID10031_H

#include "../gen/datatypes.h"
#include "parsemodule.h"

class ParseID10031 : public ParseModule
{
public:
    ParseID10031(QByteArray &BA);

    struct Point85
    {
        float An[9]; // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
        quint32 Dis;
    };

    bool Parse(int &count);

    bool ParseID85(S2DataTypes::OscHeader &OHD, const QString &fn, int &count);
};

#endif // PARSEID10031_H
