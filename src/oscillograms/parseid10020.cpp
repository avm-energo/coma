#include "parseid10020.h"

#include "trendviewmodel.h"

ParseID10020::ParseID10020(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10020::Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model)
{
    Q_UNUSED(id)
    const QStringList analogValues = model->analogValues();

    float xmin = -(header.step * 512);

    if (!model->setPointsAxis(xmin, header.step))
        return false;
    int position = 0;
    for (quint32 i = 0; i < header.len; ++i) // цикл по точкам
    {
        Point8x point;
        if (!PosPlusPlus(&point, position, sizeof(Point8x)))
            return false;
        for (size_t j = 0; j < point.An.size(); ++j)
            model->addAnalogPoint(analogValues.at(j), point.An[j]);
    }
    return true;
}
