#include "oscillograms/trendview/trendviewmodelkiv.h"

#include <gen/colors.h>
#include <oscillograms/osc_ids.h>

TrendViewModelKIV::TrendViewModelKIV(int pointsum) : TrendViewModel(pointsum)
{
}

QStringList TrendViewModelKIV::analogColors() const
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

QStringList TrendViewModelKIV::analogDescriptions() const
{
    return {
        "Напряжение ф.А, кВ", //
        "Напряжение ф.В, кВ", //
        "Напряжение ф.С, кВ", //
        "Ток ф.А, мА",        //
        "Ток ф.В, мА",        //
        "Ток ф.С, мА",        //
    };
}

QStringList TrendViewModelKIV::analogValues() const
{
    return { "UA", "UB", "UC", "IA", "IB", "IC" };
}

//////////////////////////////////////////////////////////

TrendViewModelKIVOne::TrendViewModelKIVOne(const u32 id, int pointsum) : TrendViewModel(pointsum), m_id(id)
{
}

QStringList TrendViewModelKIVOne::analogColors() const
{
    switch (m_id)
    {
    case AVTUK_KIV::OSC_A_ID:
        return { Colors::YLWCOLOR, Colors::YLLCOLOR };
    case AVTUK_KIV::OSC_B_ID:
        return { Colors::GRNCOLOR, Colors::GRLCOLOR };
    case AVTUK_KIV::OSC_C_ID:
        return { Colors::REDCOLOR, Colors::RDLCOLOR };
    default:
        return {};
    }
}

QStringList TrendViewModelKIVOne::analogDescriptions() const
{
    switch (m_id)
    {
    case AVTUK_KIV::OSC_A_ID:
        return { "Напряжение ф.А, кВ", "Ток ф.А, мА" };
    case AVTUK_KIV::OSC_B_ID:
        return { "Напряжение ф.B, кВ", "Ток ф.B, мА" };
    case AVTUK_KIV::OSC_C_ID:
        return { "Напряжение ф.C, кВ", "Ток ф.C, мА" };
    default:
        return {};
    }
}

QStringList TrendViewModelKIVOne::analogValues() const
{
    switch (m_id)
    {
    case AVTUK_KIV::OSC_A_ID:
        return { "UA", "IA" };
    case AVTUK_KIV::OSC_B_ID:
        return { "UB", "IB" };
    case AVTUK_KIV::OSC_C_ID:
        return { "UC", "IC" };
    default:
        return {};
    }
}
