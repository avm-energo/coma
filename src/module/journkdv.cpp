#include "journkdv.h"

#include "../gen/modules.h"

JournKDV::JournKDV(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent)
    : Journals(jourMap, parent)
{
}

using MeasureStruct = AVM_KDV::MeasureStruct;

QVector<QVariant> JournKDV::createMeasRecord(const char *file)
{
    AVM_KDV::MeasureStruct KDVdata;
    memcpy(&KDVdata, file, sizeof(AVM_KDV::MeasureStruct));
    //    if (KDVdata.Time != 0xFFFFFFFF)
    //            outvl = { KDVdata.NUM, TimeFunc::UnixTime32ToInvString(KDVdata.Time), KDVdata.Ueff[0],
    //            KDVdata.Ueff[1],
    //                KDVdata.Ueff[2], KDVdata.Ieff[0], KDVdata.Ieff[1], KDVdata.Ieff[2], KDVdata.Frequency, KDVdata.U0,
    //                KDVdata.U1, KDVdata.U2, KDVdata.I0, KDVdata.I1, KDVdata.I2, KDVdata.Cbush[0], KDVdata.Cbush[1],
    //                KDVdata.Cbush[2], KDVdata.Tg_d[0], KDVdata.Tg_d[1], KDVdata.Tg_d[2], KDVdata.dCbush[0],
    //                KDVdata.dCbush[1], KDVdata.dCbush[2], KDVdata.dTg_d[0], KDVdata.dTg_d[1], KDVdata.dTg_d[2],
    //                KDVdata.Iunb, KDVdata.Phy_unb, KDVdata.Tmk, KDVdata.Tamb };
    return QVector<QVariant>();
}

QVector<QVector<QVariant>> JournKDV::createMeas(const QByteArray &array)
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
    // NOTE Fix it to enable jours
    //    std::transform(data.cbegin(), data.cend(), std::back_inserter(ValueLists), [this](const MeasureStruct
    //    &kdvdata) {
    //        QVector<QVariant> outvl { kdvdata.NUM, TimeFunc::UnixTime32ToInvString(kdvdata.Time, m_timezone),
    //            kdvdata.Ueff[0], kdvdata.Ueff[1], kdvdata.Ueff[2], kdvdata.Ieff[0], kdvdata.Ieff[1], kdvdata.Ieff[2],
    //            kdvdata.Frequency, kdvdata.U0, kdvdata.U1, kdvdata.U2, kdvdata.I0, kdvdata.I1, kdvdata.I2,
    //            kdvdata.Cbush[0], kdvdata.Cbush[1], kdvdata.Cbush[2], kdvdata.Tg_d[0], kdvdata.Tg_d[1],
    //            kdvdata.Tg_d[2], kdvdata.dCbush[0], kdvdata.dCbush[1], kdvdata.dCbush[2], kdvdata.dTg_d[0],
    //            kdvdata.dTg_d[1], kdvdata.dTg_d[2], kdvdata.Iunb, kdvdata.Phy_unb, kdvdata.Tmk, kdvdata.Tamb };
    //        return outvl;
    //    });
    return ValueLists;
}

int JournKDV::measureSize()
{
    return sizeof(MeasureStruct);
}
