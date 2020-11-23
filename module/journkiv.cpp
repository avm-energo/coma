#include "journkiv.h"

#include "../gen/timefunc.h"
#include "modules.h"
JournKIV::JournKIV(QObject *parent) : Journals(parent)
{
    m_workJourDescription = AVM_KIV::workJourDescription;
}

void JournKIV::setWorkJourDescription()
{
    m_workJourDescription = AVM_KIV::workJourDescription;
}

void JournKIV::setMeasJourHeaders()
{
    m_measJourHeaders = AVM_KIV::measJourHeaders;
}

void JournKIV::setMeasRecord(char *file, QVector<QVariant> &outvl)
{
    AVM_KIV::MeasureStruct kivdata;
    memcpy(&kivdata, file, sizeof(AVM_KIV::MeasureStruct));
    if (kivdata.Time != 0xFFFFFFFF)
        outvl = { kivdata.NUM, TimeFunc::UnixTime32ToInvString(kivdata.Time), kivdata.Ueff[0], kivdata.Ueff[1],
            kivdata.Ueff[2], kivdata.Ieff[0], kivdata.Ieff[1], kivdata.Ieff[2], kivdata.Frequency, kivdata.U0,
            kivdata.U1, kivdata.U2, kivdata.I0, kivdata.I1, kivdata.I2, kivdata.Cbush[0], kivdata.Cbush[1],
            kivdata.Cbush[2], kivdata.Tg_d[0], kivdata.Tg_d[1], kivdata.Tg_d[2], kivdata.dCbush[0], kivdata.dCbush[1],
            kivdata.dCbush[2], kivdata.dTg_d[0], kivdata.dTg_d[1], kivdata.dTg_d[2], kivdata.Iunb, kivdata.Phy_unb,
            kivdata.Tmk, kivdata.Tamb };
}

int JournKIV::measureSize()
{
    return sizeof(AVM_KIV::MeasureStruct);
}

int JournKIV::workJournalID()
{
    return AVM_KIV::workJourId;
}
