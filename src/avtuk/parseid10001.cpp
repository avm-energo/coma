#include "parseid10001.h"

ParseID10001::ParseID10001(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10001::Parse(int &count)
{
    return true;
}

bool ParseID10001::Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model)
{

    if (!model->SetPointsAxis(0, header.step))
        return false;
    int position = 0;
    model->tmpav_21 << QString::number(id);
    for (quint32 i = 0; i < header.len; ++i) // цикл по точкам
    {
        Point21 point;
        if (!PosPlusPlus(&point, position, sizeof(Point21)))
            return false;
        model->AddAnalogPoint(model->tmpav_21.at(0), point.An);
    }

    return true;
}
