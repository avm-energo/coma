#include "parseid10031.h"

#include "../gen/error.h"
#include "../gen/modules.h"
#include "../gen/timefunc.h"

ParseID10031::ParseID10031(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10031::Parse(int &count)
{

    return true;
}

bool ParseID10031::ParseID85(S2DataTypes::OscHeader &OHD, const QString &fn, int &count)
{

    return true;
}

bool ParseID10031::Parse(quint32 id, S2DataTypes::OscHeader &header, TrendViewModel *model)
{

    return true;
}
