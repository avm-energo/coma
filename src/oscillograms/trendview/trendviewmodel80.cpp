#include "trendviewmodel80.h"

#include <gen/colors.h>

TrendViewModel80::TrendViewModel80(int pointsnum) : TrendViewModel(pointsnum)
{
}

QStringList TrendViewModel80::analogColors() const
{
    return {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR  //
    };
}

QStringList TrendViewModel80::digitalColors() const
{
    return {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR, //
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::BLUCOLOR, //
        Colors::BLDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR, //
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR, //
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR  //
    };
}

QStringList TrendViewModel80::analogDescriptions() const
{
    return {
        "Напряжение А, В",  //
        "Напряжение В, В",  //
        "Напряжение С, В",  //
        "Ток А, А",         //
        "Ток В, А",         //
        "Ток С, А",         //
        "Напряжение АB, В", //
        "Напряжение ВC, В", //
        "Напряжение СA, В"  //
    };
}

QStringList TrendViewModel80::digitalDescriptions() const
{
    return {};
}

QStringList TrendViewModel80::analogValues() const
{
    return { "USA", "USB", "USC", "IA", "IB", "IC", "ULA", "ULB", "ULC" };
}

QStringList TrendViewModel80::digitalValues() const
{
    return {};
}
