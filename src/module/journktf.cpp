#include "journktf.h"

#include "../gen/modules.h"
#include "../gen/timefunc.h"
JournKTF::JournKTF(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent)
    : Journals(jourMap, parent)
{
}

using MeasureStruct = AVM_KTF::MeasureStruct;

QVector<QVariant> JournKTF::createMeasRecord(const char *file)
{
    MeasureStruct ktfdata;
    memcpy(&ktfdata, file, sizeof(MeasureStruct));
    if (ktfdata.Time == 0xFFFFFFFF)
        return QVector<QVariant>();
    QVector<QVariant> outvl { ktfdata.NUM, TimeFunc::UnixTime32ToInvString(ktfdata.Time, m_timezone), ktfdata.Ueff[0],
        ktfdata.Ueff[1], ktfdata.Ueff[2], ktfdata.Ieff[0], ktfdata.Ieff[1], ktfdata.Ieff[2], ktfdata.Frequency,
        ktfdata.U0, ktfdata.U1, ktfdata.U2, ktfdata.I0, ktfdata.I1, ktfdata.I2, ktfdata.Pf[0], ktfdata.Pf[1],
        ktfdata.Pf[2], ktfdata.Pf[3], ktfdata.Qf[0], ktfdata.Qf[1], ktfdata.Qf[2], ktfdata.Qf[3], ktfdata.Sf[0],
        ktfdata.Sf[1], ktfdata.Sf[2], ktfdata.Sf[3], ktfdata.Cosphi, ktfdata.Tmk, ktfdata.Tamb, ktfdata.Twin };
    return outvl;
}

QVector<QVector<QVariant>> JournKTF::createMeas(const QByteArray &array)
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
    std::transform(data.cbegin(), data.cend(), std::back_inserter(ValueLists), [this](const MeasureStruct &ktfdata) {
        QVector<QVariant> outvl { ktfdata.NUM, TimeFunc::UnixTime32ToInvString(ktfdata.Time, m_timezone),
            ktfdata.Ueff[0], ktfdata.Ueff[1], ktfdata.Ueff[2], ktfdata.Ieff[0], ktfdata.Ieff[1], ktfdata.Ieff[2],
            ktfdata.Frequency, ktfdata.U0, ktfdata.U1, ktfdata.U2, ktfdata.I0, ktfdata.I1, ktfdata.I2, ktfdata.Pf[0],
            ktfdata.Pf[1], ktfdata.Pf[2], ktfdata.Pf[3], ktfdata.Qf[0], ktfdata.Qf[1], ktfdata.Qf[2], ktfdata.Qf[3],
            ktfdata.Sf[0], ktfdata.Sf[1], ktfdata.Sf[2], ktfdata.Sf[3], ktfdata.Cosphi, ktfdata.Tmk, ktfdata.Tamb,
            ktfdata.Twin };
        return outvl;
    });
    return ValueLists;
}

int JournKTF::measureSize()
{
    return sizeof(MeasureStruct);
}
