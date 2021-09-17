#include "parseid10030.h"

ParseID10030::ParseID10030(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10030::Parse(int &count)
{
    return true;
}

bool ParseID10030::Parse(quint32 id, S2DataTypes::OscHeader &header, TrendViewModel *model)
{
    const QStringList digitalValues { "OCNA", "OCNB", "OCNC", "OCFA", "OCFB", "OCFC", "BKCA", "BKCB", "BKCC", "BKOA",
        "BKOB", "BKOC", "CSC", "CSO", "CNA", "CNB", "CNC", "CFA", "CFB", "CFC" };
    const QStringList analogValues { "USA", "USB", "USC", "IA", "IB", "IC", "ULA", "ULB", "ULC" };

    model->processDigitalNames(digitalValues);
    model->processAnalogNames(analogValues);

    model->tmpdv_85 = digitalValues;
    model->tmpav_85 = analogValues;

    float xmin = -10;

    if (!model->SetPointsAxis(xmin, header.step))
        return false;
    int position = 0;
    for (quint32 i = 0; i < header.len; ++i) // цикл по точкам
    {
        Point85 point;
        if (!PosPlusPlus(&point, position, sizeof(Point85)))
            return false;

        quint32 DisPoint = point.Dis;
        for (int i = 0; i < model->tmpdv_85.size(); ++i)
        {
            if (DisPoint & 0x00000001)
                model->AddDigitalPoint(model->tmpdv_85.at(i), 1);
            else
                model->AddDigitalPoint(model->tmpdv_85.at(i), 0);
            DisPoint >>= 1;
        }
        for (int i = 0; i < model->tmpav_85.size(); ++i)
            model->AddAnalogPoint(model->tmpav_85.at(i), point.An[i]);
    }
}
