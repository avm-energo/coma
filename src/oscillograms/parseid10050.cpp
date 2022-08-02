#include "parseid10050.h"

#include "trendview/trendviewmodelkdv.h"

ParseID10050::ParseID10050(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10050::Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model)
{
    Q_UNUSED(id)
    TrendViewModelKDVSpec *mdl = static_cast<TrendViewModelKDVSpec *>(model);
    const auto analogValues = model->analogValues();
    // const auto digitalValues = model->digitalValues();
    model->processAnalogNames(analogValues);
    // model->processDigitalNames(digitalValues);

    if (!model->setPointsAxis(0, header.step))
        return false;

    auto position = 0;
    QVector<double> vect;
    //    vect.resize(KDVOSCPOINTS);
    for (quint32 i = 0; i < 6; ++i) // цикл по точкам
    {
        vect.clear();
        if (!PosPlusPlus(&point, position, sizeof(point)))
            return false;
        std::copy(point.cbegin(), point.cend(), std::back_inserter(vect));
        vect.remove(KDVSPECPOINTS / 2, KDVSPECPOINTS / 2);
        mdl->setAnalogData(analogValues.at(i), vect);
    }
    return true;
}
