#include "oscillograms/parseid10001.h"

ParseID10001::ParseID10001(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10001::Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model)
{
    if (!model->setPointsAxis(0, header.step))
        return false;
    int position = 0;
    model->addAnalogValue(QString::number(id));
    const auto analogValues = model->analogValues();
    for (quint32 i = 0; i < header.len; ++i) // цикл по точкам
    {
        Point21 point;
        if (!PosPlusPlus(&point, position, sizeof(Point21)))
            return false;
        if (!model->addAnalogPoint(analogValues.at(0), point.An))
            return false;
    }

    return true;
}
