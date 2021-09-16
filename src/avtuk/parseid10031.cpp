#include "parseid10031.h"

#include "../gen/error.h"
#include "../gen/modules.h"
#include "../gen/timefunc.h"

ParseID10031::ParseID10031(QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID10031::Parse(int &count)
{
    const QStringList tmpdv { "OCNA", "OCNB", "OCNC", "OCFA", "OCFB", "OCFC", "BKCA", "BKCB", "BKCC", "BKOA", "BKOB",
        "BKOC", "CSC", "CSO", "CNA", "CNB", "CNC", "CFA", "CFB", "CFC" };
    const QStringList tmpav { "USA", "USB", "USC", "IA", "IB", "IC", "ULA", "ULB", "ULC" };

    S2DataTypes::DataRecHeader DR;
    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return false;

    S2DataTypes::OscHeader OHD;
    if (!PosPlusPlus(&OHD, count, sizeof(OHD)))
        return false;

    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return false;

    // составляем имя файла осциллограммы
    QString tmps = TimeFunc::UnixTime64ToString(OHD.time);
    tmps.replace("/", "-");
    tmps.replace(":", "_");
    tmps.insert(0, "_");
    tmps.insert(0, QString::number(DR.id));

    m_trendViewModel = std::unique_ptr<TrendViewModel>(new TrendViewModel(tmpdv, tmpav, OHD.len));
    m_trendViewModel->SaveID(DR.id);
    m_trendViewModel->Len = OHD.len;
    m_trendViewModel->tmpdv_85 = tmpdv;
    m_trendViewModel->tmpav_85 = tmpav;

    switch (DR.id)
    {
    case AVTUK_85::OSC_ID:
        if (!ParseID85(OHD, tmps, count))
            return false;
        break;
    }
    return true;
}

bool ParseID10031::ParseID85(S2DataTypes::OscHeader &OHD, const QString &fn, int &count)
{

    float xmin = -10;

    if (!m_trendViewModel->SetPointsAxis(xmin, OHD.step))
        return false;
    for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
    {
        Point85 point;
        if (!PosPlusPlus(&point, count, sizeof(Point85)))
            return false;

        quint32 DisPoint = point.Dis;
        for (int i = 0; i < m_trendViewModel->tmpdv_85.size(); ++i)
        {
            if (DisPoint & 0x00000001)
                m_trendViewModel->AddDigitalPoint(m_trendViewModel->tmpdv_85.at(i), 1);
            else
                m_trendViewModel->AddDigitalPoint(m_trendViewModel->tmpdv_85.at(i), 0);
            DisPoint >>= 1;
        }
        for (int i = 0; i < m_trendViewModel->tmpav_85.size(); ++i)
            m_trendViewModel->AddAnalogPoint(m_trendViewModel->tmpav_85.at(i), point.An[i]);
    }
    m_trendViewModel->SetFilename(fn);
    return true;
}
