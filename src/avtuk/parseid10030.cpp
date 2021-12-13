#include "parseid10030.h"

#include "trendviewmodel.h"
ParseID10030::ParseID10030(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10030::Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model)
{
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
        for (int i = 0; i < digitalValues.size(); ++i)
        {
            if (DisPoint & 0x00000001)
                model->addDigitalPoint(digitalValues.at(i), 1);
            else
                model->addDigitalPoint(digitalValues.at(i), 0);
            DisPoint >>= 1;
        }
        for (int i = 0; i < analogValues.size(); ++i)
            model->addAnalogPoint(analogValues.at(i), point.An[i]);
    }
    return true;
}
