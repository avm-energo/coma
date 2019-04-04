#include <QVector>
#include "../config/config.h"
#include "eoscillogram.h"
#include "parsemodule.h"
#include "parseid9000.h"
#include "../models/trendviewmodel.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../gen/colors.h"
#include "../dialogs/trendviewdialog.h"
#include "modulebsi.h"

ParseID9000::ParseID9000(QByteArray &BA) : ParseModule(BA)
{

}

int ParseID9000::Parse(int &count)
{
    OscHeader_Data OHD;
    if (!PosPlusPlus(&OHD, count, sizeof(OHD)))
        return Error::ER_GENERALERROR;

    DataRecHeader DR;
    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return Error::ER_GENERALERROR;

    TrendViewModel::SaveID(DR.id); // для выбора
    // составляем имя файла осциллограммы
    QString tmps = TimeFunc::UnixTime64ToString(OHD.unixtime);
    tmps.replace("/","-");
    tmps.replace(":","_");
    tmps.insert(0, "_");
    tmps.insert(0, QString::number(DR.id));

    QStringList tmpav, tmpdv;
    TrendViewDialog *dlg = new TrendViewDialog(BArray);

    switch (DR.id)
    {
        case MT_ID21:
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
        case MT_ID21E:
        if (ParseID21(DR.id, OHD, tmps, dlg, count) != Error::ER_NOERROR)
            return Error::ER_GENERALERROR;
        break;

        case MT_ID80:
        if (ParseID8x(DR.id, OHD, tmps, dlg, count) != Error::ER_NOERROR)
            return Error::ER_GENERALERROR;
        break;

        case MT_ID85:
        //if (ParseID85(DR.id, OHD, tmps, tmpav, tmpdv, dlg, count) != Error::ER_NOERROR)
        //    return Error::ER_GENERALERROR;
        break;
    }
    return Error::ER_NOERROR;
}

int ParseID9000::ParseID21(quint32 id, OscHeader_Data &OHD, const QString &fn, TrendViewDialog *dlg, int &count)
{
    Q_UNUSED(id);
    if (!TModel->SetPointsAxis(0, OHD.step))
        return Error::ER_GENERALERROR;
    for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
    {
        Point21 point;
        if (!PosPlusPlus(&point, count, sizeof(Point21)))
            return Error::ER_GENERALERROR;
        TModel->AddAnalogPoint(TModel->tmpav_21.at(0), point.An);
    }
    TModel->SetFilename(fn);
    dlg->setModal(false);
    dlg->PlotShow();
    dlg->show();
    return Error::ER_NOERROR;
}

int ParseID9000::ParseID8x(quint32 id, OscHeader_Data &OHD, const QString &fn, TrendViewDialog *dlg, int &count)
{
    Q_UNUSED(id);
    //tmpav << "UA" << "UB" << "UC" << "IA" << "IB" << "IC";
   // tmpdv.clear();
//            int np = C80
    float xmax = (static_cast<float>(OHD.len/2));
    float xmin = -xmax;
    xmin = -(OHD.step * 512);

    if (!TModel->SetPointsAxis(xmin, OHD.step))
        return Error::ER_GENERALERROR;
    for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
    {
        Point8x point;
        if (!PosPlusPlus(&point, count, sizeof(Point8x)))
            return Error::ER_GENERALERROR;
        for (int i=0; i<6; ++i)
            TModel->AddAnalogPoint(TModel->tmpav_80.at(i), point.An[i]);
    }
    TModel->SetFilename(fn);
    dlg->setModal(false);
    dlg->PlotShow();
    dlg->show();
    return Error::ER_NOERROR;
}

int ParseID9000::ParseID85(quint32 id, OscHeader_Data &OHD, const QString &fn, TrendViewDialog *dlg, int &count)
{
    Q_UNUSED(id);
    Q_UNUSED(count);
    Q_UNUSED(dlg);
    Q_UNUSED(fn);
    Q_UNUSED(OHD);
    /*tmpdv << "OCNA" << "OCNB" << "OCNC" << "OCFA" << "OCFB" << "OCFC" << \
             "BKCA" << "BKCB" << "BKCC" << "BKOA" << "BKOB" << "BKOC" << \
             "CSC" << "CSO" << "CNA" << "CNB" << "CNC" << "CFA" << "CFB" << "CFC" << \
             "nNA" << "nNB" << "nNC" << "nFA" << "nFB" << "nFC" << "nCA" << "nCB" << "nCC" << \
             "nOA" << "nOB" << "nOC";
    tmpav << "USA" << "USB" << "USC" << "IA" << "IB" << "IC" << "ULA" << "ULB" << "ULC";
    float xmax = (static_cast<float>(OHD.len/2));
    float xmin = -xmax;
    xmin = -(OHD.step * 512);
    TrendViewModel *TModel = new TrendViewModel(tmpdv, tmpav, OHD.len);
    dlg->SetModel(TModel);
    dlg->SetAnalogNames(tmpav);
    dlg->SetDigitalNames(tmpdv);
    QStringList acolors = QStringList() << YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                           YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                           YLDCOLOR << GRDCOLOR << RDDCOLOR;
    QStringList dcolors = QStringList() << YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                           YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                           YLDCOLOR << GRDCOLOR << RDDCOLOR << \
                                           YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                           BLUCOLOR << BLDCOLOR << \
                                           YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                           YLDCOLOR << GRDCOLOR << RDDCOLOR << \
                                           YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                           YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                           YLDCOLOR << GRDCOLOR << RDDCOLOR << \
                                           YLWCOLOR << GRNCOLOR << REDCOLOR;
    dlg->SetDigitalColors(dcolors);
    dlg->SetAnalogColors(acolors);
    dlg->SetRanges(xmin, xmax, -200, 200);
    dlg->SetupPlots();
    dlg->SetupUI();
    if (!TModel->SetPointsAxis(xmin, OHD.step))
        return Error::ER_GENERALERROR;
    for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
    {
        Point85 point;
        if (!PosPlusPlus(&point, count, sizeof(Point85)))
            return Error::ER_GENERALERROR;
//                quint32 DisPoint = point.Dis & 0x000FFFFF; // оставляем только младшие 20 бит
        quint32 DisPoint = point.Dis;
        for (int i=0; i<32; ++i)
        {
            if (DisPoint & 0x00000001)
                TModel->AddDigitalPoint(tmpdv.at(i), 1);
            else
                TModel->AddDigitalPoint(tmpdv.at(i), 0);
            DisPoint >>= 1;
        }
        for (int i=0; i<9; ++i)
            TModel->AddAnalogPoint(tmpav.at(i), point.An[i]);
    }
    TModel->SetFilename(fn);
    dlg->setModal(false);
    dlg->PlotShow();
    dlg->show();*/
    return Error::ER_NOERROR;
}


/*void ParseID9000::Save(quint32 *len)
{


}*/
