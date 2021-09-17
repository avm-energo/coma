#ifndef PARSEID9000_H
#define PARSEID9000_H

#include "../gen/datatypes.h"
#include "parsemodule.h"

class ParseID9000 : public ParseModule
{
public:
    ParseID9000(const QByteArray &BA);

    bool Parse(int &count);
    bool ParseID21(quint32 id, S2DataTypes::OscHeader &OHD, int &count);
    bool ParseID8x(quint32 id, S2DataTypes::OscHeader &OHD, TrendViewDialog *dlg, int &count);
    bool ParseID85(quint32 id, S2DataTypes::OscHeader &OHD, TrendViewDialog *dlg, int &count);
    bool Parse(quint32 id, S2DataTypes::OscHeader &header, TrendViewModel *model) override
    {
        return true;
    }
};

#endif // PARSEID9000_H
