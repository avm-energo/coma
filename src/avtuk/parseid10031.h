#ifndef PARSEID10031_H
#define PARSEID10031_H

#include "../gen/datatypes.h"
#include "parsemodule.h"

class ParseID10031 : public ParseModule
{
public:
    ParseID10031(const QByteArray &BA);

    bool Parse(int &count);

    bool ParseID85(S2DataTypes::OscHeader &OHD, const QString &fn, int &count);
    bool Parse(quint32 id, S2DataTypes::OscHeader &header, TrendViewModel *model) override;
};

#endif // PARSEID10031_H
