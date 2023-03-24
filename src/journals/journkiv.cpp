#include "journkiv.h"

#include <gen/timefunc.h>

JournKIV::JournKIV(const ModuleTypes::JourMap &jourMap, QObject *parent) : Journals(jourMap, parent)
{
}

using MeasureStruct = AVM_KIV::MeasureStruct;

QVector<QVariant> JournKIV::createMeasRecord(const char *file)
{
    MeasureStruct kivdata;
    memcpy(&kivdata, file, sizeof(MeasureStruct));
    if (kivdata.Time == 0xFFFFFFFF)
        return QVector<QVariant>();
    QVector<QVariant> outvl { kivdata.NUM, TimeFunc::UnixTime32ToInvString(kivdata.Time, m_timezone), kivdata.Ueff[0],
        kivdata.Ueff[1], kivdata.Ueff[2], kivdata.Ieff[0], kivdata.Ieff[1], kivdata.Ieff[2], kivdata.Frequency,
        kivdata.U0, kivdata.U1, kivdata.U2, kivdata.I0, kivdata.I1, kivdata.I2, kivdata.Cbush[0], kivdata.Cbush[1],
        kivdata.Cbush[2], kivdata.Tg_d[0], kivdata.Tg_d[1], kivdata.Tg_d[2], kivdata.dCbush[0], kivdata.dCbush[1],
        kivdata.dCbush[2], kivdata.dTg_d[0], kivdata.dTg_d[1], kivdata.dTg_d[2], kivdata.Iunb, kivdata.Phy_unb,
        kivdata.Tmk, kivdata.Tamb };
    return outvl;
}

QVector<QVector<QVariant>> JournKIV::createMeas(const QByteArray &array)
{
    const auto basize = array.size();
    const char *file = array.constData();
    const auto recordsize = measureSize();

    QVector<MeasureStruct> data;
    for (int i = 0; i < basize; i += recordsize)
    {
        MeasureStruct kivdata;
        memcpy(&kivdata, file, sizeof(MeasureStruct));
        file += recordsize;
        if (kivdata.Time == 0xFFFFFFFF)
            continue;
        data.append(kivdata);
    }
    std::sort(data.begin(), data.end(),
        [](const MeasureStruct &lhs, const MeasureStruct &rhs) { return lhs.Time > rhs.Time; });
    QVector<QVector<QVariant>> ValueLists;
    std::transform(data.cbegin(), data.cend(), std::back_inserter(ValueLists), [this](const MeasureStruct &kivdata) {
        QVector<QVariant> outvl { kivdata.NUM, TimeFunc::UnixTime32ToInvString(kivdata.Time, m_timezone),
            kivdata.Ueff[0], kivdata.Ueff[1], kivdata.Ueff[2], kivdata.Ieff[0], kivdata.Ieff[1], kivdata.Ieff[2],
            kivdata.Frequency, kivdata.U0, kivdata.U1, kivdata.U2, kivdata.I0, kivdata.I1, kivdata.I2, kivdata.Cbush[0],
            kivdata.Cbush[1], kivdata.Cbush[2], kivdata.Tg_d[0], kivdata.Tg_d[1], kivdata.Tg_d[2], kivdata.dCbush[0],
            kivdata.dCbush[1], kivdata.dCbush[2], kivdata.dTg_d[0], kivdata.dTg_d[1], kivdata.dTg_d[2], kivdata.Iunb,
            kivdata.Phy_unb, kivdata.Tmk, kivdata.Tamb };
        return outvl;
    });
    return ValueLists;
}

int JournKIV::measureSize()
{
    return sizeof(MeasureStruct);
}
