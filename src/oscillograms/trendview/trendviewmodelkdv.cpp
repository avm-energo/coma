#include "trendviewmodelkdv.h"

#include "../gen/colors.h"

TrendViewModelKDV::TrendViewModelKDV(int pointsnum) : TrendViewModel(pointsnum)
{
}

QStringList TrendViewModelKDV::analogColors() const
{
    return { Colors::YLWCOLOR, //
        Colors::GRNCOLOR,      //
        Colors::REDCOLOR,      //
        Colors::YLLCOLOR,      //
        Colors::GRLCOLOR,      //
        Colors::RDLCOLOR };
}

QStringList TrendViewModelKDV::analogDescriptions() const
{
    return { "Напряжение А, В", //
        "Напряжение В, В",      //
        "Напряжение С, В",      //
        "Ток А, А",             //
        "Ток В, А",             //
        "Ток С, А" };
}

QStringList TrendViewModelKDV::analogValues() const
{
    return { "UA", "UB", "UC", "IA", "IB", "IC" };
}

QString TrendViewModelKDV::xAxisDescription() const
{
    return "Время, мс";
}

void TrendViewModelKDV::setAnalogData(const QString &name, QVector<double> &data)
{
    m_analogMainData.insert(name, data);
}

TrendViewModelKDVV::TrendViewModelKDVV(int pointsnum) : TrendViewModelKDV(pointsnum)
{
}

QStringList TrendViewModelKDVV::analogDescriptions() const
{
    return { "Виброперемещение кан. 1, мкм", //
        "Виброперемещение кан. 2, мкм",      //
        "Виброперемещение кан. 3, мкм",      //
        "Виброперемещение кан. 4, мкм",      //
        "Виброперемещение кан. 5, мкм",      //
        "Виброперемещение кан. 6, мкм" };
}

QStringList TrendViewModelKDVV::analogValues() const
{
    return { "V1", "V2", "V3", "V4", "V5", "V6" };
}

TrendViewModelKDVSpec::TrendViewModelKDVSpec(int pointsnum) : TrendViewModel(pointsnum)
{
}

QStringList TrendViewModelKDVSpec::analogColors() const
{
    return { Colors::BLDCOLOR, //
        Colors::BLUCOLOR,      //
        Colors::BLLCOLOR,      //
        Colors::RDDCOLOR,      //
        Colors::REDCOLOR,      //
        Colors::RDLCOLOR };
}

QStringList TrendViewModelKDVSpec::analogDescriptions() const
{
    return {
        "Ua, дБ", //
        "Ub, дБ", //
        "Uc, дБ", //
        "Ia, дБ", //
        "Ib, дБ", //
        "Ic, дБ"  //
    };
}

QStringList TrendViewModelKDVSpec::analogValues() const
{
    return { "SUA", "SUB", "SUC", "SIA", "SIB", "SIC" };
}

QString TrendViewModelKDVSpec::xAxisDescription() const
{
    return "Частота, Гц";
}

void TrendViewModelKDVSpec::setAnalogData(const QString &name, QVector<double> &data)
{
    m_analogMainData.insert(name, data);
}

TrendViewModelKDVVSpec::TrendViewModelKDVVSpec(int pointsnum) : TrendViewModelKDVSpec(pointsnum)
{
}

QStringList TrendViewModelKDVVSpec::analogDescriptions() const
{
    return { "Виброперемещение кан. 1, дБ", //
        "Виброперемещение кан. 2, дБ",      //
        "Виброперемещение кан. 3, дБ",      //
        "Виброперемещение кан. 4, дБ",      //
        "Виброперемещение кан. 5, дБ",      //
        "Виброперемещение кан. 6, дБ" };
}

QStringList TrendViewModelKDVVSpec::analogValues() const
{
    return { "SV1", "SV2", "SV3", "SV4", "SV5", "SV6" };
}
