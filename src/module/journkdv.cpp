#include "journkdv.h"

#include "modules.h"

JournKDV::JournKDV(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent)
    : Journals(jourMap, parent)
{
}

QVector<QVariant> JournKDV::createMeasRecord(const char *file)
{
    AVM_KDV::MeasureStruct KDVdata;
    memcpy(&KDVdata, file, sizeof(AVM_KDV::MeasureStruct));
    //    if (KDVdata.Time != 0xFFFFFFFF)
    //        outvl = { KDVdata.NUM, TimeFunc::UnixTime32ToInvString(KDVdata.Time), KDVdata.Ueff[0], KDVdata.Ueff[1],
    //            KDVdata.Ueff[2], KDVdata.Ieff[0], KDVdata.Ieff[1], KDVdata.Ieff[2], KDVdata.Frequency, KDVdata.U0,
    //            KDVdata.U1, KDVdata.U2, KDVdata.I0, KDVdata.I1, KDVdata.I2, KDVdata.Cbush[0], KDVdata.Cbush[1],
    //            KDVdata.Cbush[2], KDVdata.Tg_d[0], KDVdata.Tg_d[1], KDVdata.Tg_d[2], KDVdata.dCbush[0],
    //            KDVdata.dCbush[1], KDVdata.dCbush[2], KDVdata.dTg_d[0], KDVdata.dTg_d[1], KDVdata.dTg_d[2],
    //            KDVdata.Iunb, KDVdata.Phy_unb, KDVdata.Tmk, KDVdata.Tamb };
    return QVector<QVariant>();
}

int JournKDV::measureSize()
{
    return sizeof(AVM_KDV::MeasureStruct);
}
