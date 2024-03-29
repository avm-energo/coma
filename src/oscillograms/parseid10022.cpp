#include "parseid10022.h"

#include "trendview/trendviewmodel.h"

ParseID10022::ParseID10022(const QByteArray &ba) : ParseModule(ba)
{
}

bool ParseID10022::Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model)
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
        for (std::size_t j = 0; j < point.data.size(); ++j)
        {
            if (!model->addAnalogPoint(analogValues.at(j), point.data[j]))
                return false;
        }
    }
    return true;
}
