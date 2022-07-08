#include "parseid10040.h"

#include "trendview/trendviewmodel.h"

ParseID10040::ParseID10040(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10040::Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model)
{
    Q_UNUSED(id)
    const auto analogValues = model->analogValues();
    // const auto digitalValues = model->digitalValues();
    model->processAnalogNames(analogValues);
    // model->processDigitalNames(digitalValues);

    auto xmin = -(header.step * 512);
    if (!model->setPointsAxis(xmin, header.step))
        return false;

    auto position = 0;
    for (quint32 i = 0; i < header.len; ++i) // цикл по точкам
    {
        PointKDV point;
        if (!PosPlusPlus(&point, position, sizeof(PointKDV)))
            return false;
        for (std::size_t j = 0; j < point.An.size(); ++j)
        {
            if (!model->addAnalogPoint(analogValues.at(j), point.An[j]))
                return false;
        }
    }
    return true;
}
