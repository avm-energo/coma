#include "parseid9000.h"

#include "../gen/error.h"
#include "../gen/modules.h"
#include "../gen/timefunc.h"
#include "trendviewdialog.h"

ParseID9000::ParseID9000(const QByteArray &BA) : ParseModule(BA)
{
}

bool ParseID9000::Parse(int &count)
{
    S2DataTypes::OscHeader OHD;
    if (!PosPlusPlus(&OHD, count, sizeof(OHD)))
        return false;

    S2DataTypes::DataRecHeader DR;
    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return false;
    {
        // составляем имя файла осциллограммы
        QString tmps = TimeFunc::UnixTime64ToString(OHD.time);
        tmps.replace("/", "-");
        tmps.replace(":", "_");
        tmps.insert(0, "_");
        tmps.insert(0, QString::number(DR.id));
        m_trendViewModel->SetFilename(tmps);
    }
    QStringList tmpav, tmpdv;
    m_trendViewModel = std::unique_ptr<TrendViewModel>(new TrendViewModel(OHD.len));
    m_trendViewModel->SaveID(DR.id);

    m_trendViewModel->Len = OHD.len;
    m_trendViewModel->xmax = static_cast<float>(m_trendViewModel->Len / 2);
    m_trendViewModel->xmin = -m_trendViewModel->xmax;

    switch (DR.id)
    {
    case AVTUK_21::OSC_ID_MIN:
    case 10002:
    case 10003:
    case 10004:
    case 10005:
    case 10006:
    case 10007:
    case 10008:
    case 10009:
    case 10010:
    case 10011:
    case 10012:
    case 10013:
    case 10014:
    case 10015:
    case AVTUK_21::OSC_ID_MAX:
        if (!ParseID21(DR.id, OHD, count))
            return false;
        break;

    case AVTUK_8X::OSC_ID:
        if (!ParseID8x(DR.id, OHD, m_trendViewDialog, count))
            return false;
        break;

    case AVTUK_85::OSC_ID:
        // if (ParseID85(DR.id, OHD, tmps, tmpav, tmpdv, dlg, count) != Error::ER_NOERROR)
        //    return Error::ER_GENERALERROR;
        break;
    }
    return true;
}

bool ParseID9000::ParseID21(quint32 id, S2DataTypes::OscHeader &OHD, int &count)
{
    Q_UNUSED(id);
    //    if (!m_trendViewModel->SetPointsAxis(0, OHD.step))
    //        return false;

    //    m_trendViewModel->tmpav_21 << QString::number(m_trendViewModel->idOsc);
    //    for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
    //    {
    //        Point21 point;
    //        if (!PosPlusPlus(&point, count, sizeof(Point21)))
    //            return false;
    //        m_trendViewModel->AddAnalogPoint(m_trendViewModel->tmpav_21.at(0), point.An);
    //    }

    return true;
}

bool ParseID9000::ParseID8x(quint32 id, S2DataTypes::OscHeader &OHD, TrendViewDialog *dlg, int &count)
{
    Q_UNUSED(id);

    //    float xmin = -(OHD.step * 512);

    //    if (!m_trendViewModel->SetPointsAxis(xmin, OHD.step))
    //        return false;
    //    for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
    //    {
    //        Point8x point;
    //        if (!PosPlusPlus(&point, count, sizeof(Point8x)))
    //            return false;
    //        for (int i = 0; i < 6; ++i)
    //            m_trendViewModel->AddAnalogPoint(m_trendViewModel->tmpav_80.at(i), point.An[i]);
    //    }

    //    dlg->setModal(false);
    //    dlg->showPlot();
    //    dlg->show();
    return true;
}

bool ParseID9000::ParseID85(quint32 id, S2DataTypes::OscHeader &OHD, TrendViewDialog *dlg, int &count)
{
    Q_UNUSED(id);
    Q_UNUSED(count);
    Q_UNUSED(dlg);

    Q_UNUSED(OHD);

    return true;
}
