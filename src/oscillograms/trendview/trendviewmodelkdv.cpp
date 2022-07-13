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
    return "Время, мкс";
}

void TrendViewModelKDV::setAnalogData(const QString &name, QVector<double> &data)
{
    m_analogMainData.insert(name, data);
}

TrendViewModelKDVSpec::TrendViewModelKDVSpec(int pointsnum) : TrendViewModel(pointsnum)
{
}

QStringList TrendViewModelKDVSpec::analogColors() const
{
    return {
        Colors::BLDCOLOR, //
        Colors::BLUCOLOR, //
        Colors::BLLCOLOR, //
        Colors::RDDCOLOR, //
        Colors::REDCOLOR, //
        Colors::RDLCOLOR,
        Colors::YLDCOLOR //
    };
}

QStringList TrendViewModelKDVSpec::analogDescriptions() const
{
    return { "Канал 1, мкм", //
        "Канал 2, мкм",      //
        "Канал 3, мкм",      //
        "Канал 4, мкм",      //
        "Канал 5, мкм",      //
        "Канал 6, мкм",      //
        "Частота, Гц" };
}

QStringList TrendViewModelKDVSpec::analogValues() const
{
    return { "S1", "S2", "S3", "SA", "SB", "SC", "F" };
}

QString TrendViewModelKDVSpec::xAxisDescription() const
{
    return "Частота, Гц";
}

void TrendViewModelKDVSpec::setAnalogData(const QString &name, QVector<double> &data)
{
    m_analogMainData.insert(name, data);
}
