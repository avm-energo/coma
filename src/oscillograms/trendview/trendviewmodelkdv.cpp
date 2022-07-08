#include "trendviewmodelkdv.h"

#include "../gen/colors.h"

TrendViewModelKDV::TrendViewModelKDV(int pointsnum) : TrendViewModel(pointsnum)
{
}

QStringList TrendViewModelKDV::analogColors() const
{
    return {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
    };
}

QStringList TrendViewModelKDV::analogDescriptions() const
{
    return {
        "Напряжение А, В", //
        "Напряжение В, В", //
        "Напряжение С, В", //
        "Ток А, А",        //
        "Ток В, А",        //
        "Ток С, А"         //
    };
}

QStringList TrendViewModelKDV::analogValues() const
{
    return { "UA", "UB", "UC", "IA", "IB", "IC" };
}

QString TrendViewModelKDV::xAxisDescription() const
{
    return "Время, мкс";
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
        Colors::YLDCOLOR, //
        Colors::YLWCOLOR, //
        Colors::YLLCOLOR, //
    };
}

QStringList TrendViewModelKDVSpec::analogDescriptions() const
{
    return {
        "Канал 1, мкм", //
        "Канал 2, мкм", //
        "Канал 3, мкм", //
        "Канал 4, мкм", //
        "Канал 5, мкм", //
        "Канал 6, мкм", //
    };
}

QStringList TrendViewModelKDVSpec::analogValues() const
{
    return { "S1", "S2", "S3", "SA", "SB", "SC" };
}

QString TrendViewModelKDVSpec::xAxisDescription() const
{
    return "Частота, Гц";
}
