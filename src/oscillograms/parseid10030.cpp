#include "parseid10030.h"

#include "trendview/trendviewmodel.h"

ParseID10030::ParseID10030(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10030::Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model)
{
    Q_UNUSED(id)
    const QStringList digitalValues = model->digitalValues();
    const QStringList analogValues = model->analogValues();
    model->processDigitalNames(digitalValues);
    model->processAnalogNames(analogValues);

    float xmin = -10;

    if (!model->setPointsAxis(xmin, header.step))
        return false;
    int position = 0;
    for (quint32 i = 0; i < header.len; ++i) // цикл по точкам
    {
        Point85 point;
        if (!PosPlusPlus(&point, position, sizeof(Point85)))
            return false;

        quint32 DisPoint = point.Dis;
        for (int j = 0; j < digitalValues.size(); ++j)
        {
            if (DisPoint & 0x00000001)
                model->addDigitalPoint(digitalValues.at(j), 1);
            else
                model->addDigitalPoint(digitalValues.at(j), 0);
            DisPoint >>= 1;
        }
        for (int j = 0; j < analogValues.size(); ++j)
        {
            if (!model->addAnalogPoint(analogValues.at(j), point.An[j]))
                return false;
        }
    }
    return true;
}
