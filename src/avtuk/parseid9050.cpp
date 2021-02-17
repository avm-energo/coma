#include "parseid9050.h"

#include "../config/config.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "eoscillogram.h"
#include "parsemodule.h"
#include "trendviewdialog.h"
#include "trendviewmodel.h"

#include <QVector>

ParseID9050::ParseID9050(QByteArray &BA) : ParseModule(BA)
{
    Channel.insert("Channel_0", ID_OSC_CH0);
    Channel.insert("Channel_0", ID_SPECTR_CH0);
    Channel.insert("Channel_1", ID_OSC_CH1);
    Channel.insert("Channel_1", ID_SPECTR_CH1);
    Channel.insert("Channel_2", ID_OSC_CH2);
    Channel.insert("Channel_2", ID_SPECTR_CH2);
    Channel.insert("Channel_3", ID_OSC_CH3);
    Channel.insert("Channel_3", ID_SPECTR_CH3);
    Channel.insert("Channel_4", ID_OSC_CH4);
    Channel.insert("Channel_4", ID_SPECTR_CH4);
    Channel.insert("Channel_5", ID_OSC_CH5);
    Channel.insert("Channel_5", ID_SPECTR_CH5);
    Channel.insert("Channel_6", ID_OSC_CH6);
    Channel.insert("Channel_6", ID_SPECTR_CH6);
    Channel.insert("Channel_7", ID_OSC_CH7);
    Channel.insert("Channel_7", ID_SPECTR_CH7);
}

bool ParseID9050::Parse(int &count)
{
    // Разбор осциллограммы
    S2DataTypes::OscHeader_Data OHD;
    if (!PosPlusPlus(&OHD, count, sizeof(OHD)))
        return false;

    S2DataTypes::DataRecHeader DR;
    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return false;

    //  TrendViewModel::SaveID(DR.id); // для выбора
    // составляем имя файла осциллограммы
    QString tmps = TimeFunc::UnixTime64ToString(OHD.unixtime);
    tmps.replace("/", "-");
    tmps.replace(":", "_");
    tmps.insert(0, "_");
    tmps.insert(0, "Oscillogram");
    tmps.insert(0, "_");
    tmps.insert(0, Channel.key(DR.id));

    QStringList tmpav;
    TrendViewDialog *dlg = new TrendViewDialog(BArray);
    TModel->SaveID(DR.id);
    if (!ParseOsc(DR.id, OHD, tmps, tmpav, dlg, count))
        return false;

    // Разбор спектрограммы
    SpectHeader_Data SHD;
    if (!PosPlusPlus(&SHD, count, sizeof(SHD)))
        return false;

    if (!PosPlusPlus(&DR, count, sizeof(DR)))
        return false;

    tmps.replace("Oscillogram", "Spectrogram");

    if (!ParseSpectr(DR.id, SHD, tmps, tmpav, dlg, count))
        return false;

    return true;
}

bool ParseID9050::ParseOsc(quint32 id, S2DataTypes::OscHeader_Data &OHD, const QString &fn, QStringList tmpav,
    TrendViewDialog *dlg, int &count)
{
    tmpav << Channel.key(id);
    TrendViewModel *TModel = new TrendViewModel(QStringList(), tmpav, OHD.len);
    dlg->SetModel(TModel);
    dlg->SetAnalogNames(tmpav);
    dlg->SetRanges(0, 10000, -1250,
        1250); // 10000 мкс, 1250 мВ (сделать автонастройку в зависимости от конфигурации по данному каналу)
    dlg->SetupPlots();
    dlg->SetupUI();
    if (!TModel->SetPointsAxis(0, OHD.step))
        return false;
    for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
    {
        Point87 point;
        if (!PosPlusPlus(&point, count, sizeof(Point87)))
            return false;
        TModel->AddAnalogPoint(tmpav.at(0), point.An);
    }
    TModel->SetFilename(fn);
    dlg->setModal(false);
    dlg->PlotShow();
    dlg->show();

    return true;
}

bool ParseID9050::ParseSpectr(quint32 id, ParseID9050::SpectHeader_Data &SHD, const QString &fn, QStringList tmpav,
    TrendViewDialog *dlg, int &count)
{
    tmpav << Channel.key(id);
    TrendViewModel *TModel = new TrendViewModel(QStringList(), tmpav, SHD.len);
    dlg->SetModel(TModel);
    dlg->SetAnalogNames(tmpav);
    dlg->SetRanges(
        0, 1000000, 0, 2); // 1000000 Гц, 2 о.е. (сделать автонастройку в зависимости от конфигурации по данному каналу)
    dlg->SetupPlots();
    dlg->SetupUI();
    if (!TModel->SetPointsAxis(0, SHD.step))
        return false;
    for (quint32 i = 0; i < SHD.len; ++i) // цикл по точкам
    {
        Point87 point;
        if (!PosPlusPlus(&point, count, sizeof(Point87)))
            return false;
        TModel->AddAnalogPoint(tmpav.at(0), point.An);
    }
    TModel->SetFilename(fn);
    dlg->setModal(false);
    dlg->PlotShow();
    dlg->show();

    return true;
}

/*void ParseID9050::Save(quint32 *len)
{


}
*/
