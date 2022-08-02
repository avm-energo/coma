#include "parseid10040.h"

#include "trendview/trendviewmodelkdv.h"

ParseID10040::ParseID10040(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10040::Parse(quint32 id, const S2DataTypes::OscHeader &header, TrendViewModel *model)
{
    Q_UNUSED(id)
    TrendViewModelKDV *mdl = static_cast<TrendViewModelKDV *>(model);
    const auto analogValues = model->analogValues();
    // const auto digitalValues = model->digitalValues();
    model->processAnalogNames(analogValues);
    // model->processDigitalNames(digitalValues);

    if (!model->setPointsAxis(0, header.step))
        return false;

    auto position = 0;
    //    point.resize(KDVOSCPOINTS);
    QVector<double> vect;
    //    vect.resize(KDVOSCPOINTS);
    for (quint32 i = 0; i < 6; ++i) // цикл по точкам
    {
        //        PointKDV point;
        vect.clear();
        if (!PosPlusPlus(&point, position, sizeof(point)))
            return false;
        std::copy(point.cbegin(), point.cend(), std::back_inserter(vect));
        mdl->setAnalogData(analogValues.at(i), vect);
    }
    return true;
}
