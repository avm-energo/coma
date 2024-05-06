#include "oscillograms/parseid10023.h"

ParseID10023::ParseID10023(const QByteArray &ba) : ParseModule(ba)
{
}

bool ParseID10023::Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model)
{
    Q_UNUSED(id);
    const auto analogValues = model->analogValues();
    model->processAnalogNames(analogValues);
    float xmin = 0.0f;
    if (!model->setPointsAxis(xmin, header.step))
        return false;

    auto position = 0;
    for (quint32 i = 0; i < header.len; ++i) // цикл по точкам
    {
        DataPoint point;
        if (!PosPlusPlus(&point, position, sizeof(point)))
            return false;
        if (!model->addAnalogPoint(analogValues[0], point.U))
            return false;
        if (!model->addAnalogPoint(analogValues[1], point.I))
            return false;
    }
    return true;
}
