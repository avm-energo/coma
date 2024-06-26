#include "oscillograms/parseid10020.h"

ParseID10020::ParseID10020(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10020::Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model)
{
    Q_UNUSED(id)
    const auto analogValues = model->analogValues();
    model->processAnalogNames(analogValues);

    auto xmin = -(header.step * 512);
    if (!model->setPointsAxis(xmin, header.step))
        return false;

    auto position = 0;
    for (quint32 i = 0; i < header.len; ++i) // цикл по точкам
    {
        Point8x point;
        if (!PosPlusPlus(&point, position, sizeof(Point8x)))
            return false;
        for (std::size_t j = 0; j < point.An.size(); ++j)
        {
            if (!model->addAnalogPoint(analogValues.at(j), point.An[j]))
                return false;
        }
    }
    return true;
}
