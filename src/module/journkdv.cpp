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
    if (KDVdata.Time == 0xFFFFFFFF)
        return QVector<QVariant>();

    QVector<QVariant> outvl = { KDVdata.NUM, TimeFunc::UnixTime32ToInvString(KDVdata.Time, m_timezone), KDVdata.Ueff[0],
        KDVdata.Ueff[1], KDVdata.Ueff[2], KDVdata.Ieff[0], KDVdata.Ieff[1], KDVdata.Ieff[2], KDVdata.Frequency,
        KDVdata.U1, KDVdata.U2, KDVdata.I1, KDVdata.I2, KDVdata.Pf_sum, KDVdata.Qf_sum, KDVdata.Sf_sum, KDVdata.CosPhi,
        KDVdata.Tmk, KDVdata.Tamb, KDVdata.Twin, KDVdata.VibrSCZ_V[0], KDVdata.VibrSCZ_V[1], KDVdata.VibrSCZ_V[2],
        KDVdata.VibrSCZ_V[3], KDVdata.VibrSCZ_V[4], KDVdata.VibrSCZ_V[5], KDVdata.VibrSCZ_D[0], KDVdata.VibrSCZ_D[1],
        KDVdata.VibrSCZ_D[2], KDVdata.VibrSCZ_D[3], KDVdata.VibrSCZ_D[4], KDVdata.VibrSCZ_D[5] };
    return outvl;
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

    std::transform(data.cbegin(), data.cend(), std::back_inserter(ValueLists), [this](const MeasureStruct &kdvdata) {
        QVector<QVariant> outvl { kdvdata.NUM, TimeFunc::UnixTime32ToInvString(kdvdata.Time, m_timezone),
            kdvdata.Ueff[0], kdvdata.Ueff[1], kdvdata.Ueff[2], kdvdata.Ieff[0], kdvdata.Ieff[1], kdvdata.Ieff[2],
            kdvdata.Frequency, kdvdata.U1, kdvdata.U2, kdvdata.I1, kdvdata.I2, kdvdata.Pf_sum, kdvdata.Qf_sum,
            kdvdata.Sf_sum, kdvdata.CosPhi, kdvdata.Tmk, kdvdata.Tamb, kdvdata.Twin, kdvdata.VibrSCZ_V[0],
            kdvdata.VibrSCZ_V[1], kdvdata.VibrSCZ_V[2], kdvdata.VibrSCZ_V[3], kdvdata.VibrSCZ_V[4],
            kdvdata.VibrSCZ_V[5], kdvdata.VibrSCZ_D[0], kdvdata.VibrSCZ_D[1], kdvdata.VibrSCZ_D[2],
            kdvdata.VibrSCZ_D[3], kdvdata.VibrSCZ_D[4], kdvdata.VibrSCZ_D[5] };
        return outvl;
    });
    return ValueLists;
}

int JournKDV::measureSize()
{
    return sizeof(MeasureStruct);
}
