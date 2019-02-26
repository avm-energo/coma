#include <QVector>
#include "../config/config.h"
#include "eoscillogram.h"
#include "parsemodule.h"
#include "parseid10031.h"
#include "../models/trendviewmodel.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../gen/colors.h"
#include "../dialogs/trendviewdialog.h"
#include "modulebsi.h"

quint32 ParseID10031::len;

ParseID10031::ParseID10031(QByteArray &BA) : ParseModule(BA)
{

}

int ParseID10031::Parse(int &count)
{
    //ParseID10031::SWJournalRecordStruct SWJ;
    //PosPlusPlus(&SWJ, count, sizeof(SWJ));
    DataRecHeader DR;
    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return Error::ER_GENERALERROR;

    OscHeader_Data OHD;
    if (!PosPlusPlus(&OHD, count, sizeof(OHD)))
        return Error::ER_GENERALERROR;
    ParseID10031::len = OHD.len;

    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return Error::ER_GENERALERROR;

    TrendViewModel::SaveID(DR.id); // для выбора
    // составляем имя файла осциллограммы
    QString tmps = TimeFunc::UnixTime64ToString(OHD.unixtime);
    tmps.replace("/","-");
    tmps.replace(":","_");
    tmps.insert(0, "_");
    tmps.insert(0, QString::number(DR.id));

    switch (DR.id)
    {
        case MT_ID85:
        if (ParseID85(OHD, tmps, count) != Error::ER_NOERROR)
            return Error::ER_GENERALERROR;
        break;
    }
    return Error::ER_NOERROR;

}


int ParseID10031::ParseID85(OscHeader_Data &OHD, const QString &fn, int &count)
{

    float xmin = -(static_cast<float>(OHD.len/2));

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
                TModel->AddDigitalPoint(TModel->tmpdv_85.at(i), 1);
            else
                TModel->AddDigitalPoint(TModel->tmpdv_85.at(i), 0);
            DisPoint >>= 1;
        }
        for (int i=0; i<9; ++i)
            TModel->AddAnalogPoint(TModel->tmpav_85.at(i), point.An[i]);
    }
    TModel->SetFilename(fn);
    return Error::ER_NOERROR;
}

void ParseID10031::Save(quint32 &len)
{
    len = ParseID10031::len;
}
