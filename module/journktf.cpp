#include "journktf.h"

#include "../gen/timefunc.h"
#include "modules.h"
JournKTF::JournKTF(QObject *parent) : Journals(parent)
{
}

void JournKTF::setWorkJourDescription()
{
    m_workJourDescription = AVM_KTF::workJourDescription;
}

void JournKTF::setMeasJourHeaders()
{
    m_measJourHeaders = AVM_KTF::measJourHeaders;
}

void JournKTF::setMeasRecord(char *file, QVector<QVariant> &outvl)
{
    AVM_KTF::MeasureStruct ktfdata;
    memcpy(&ktfdata, file, sizeof(AVM_KTF::MeasureStruct));
    if (ktfdata.Time != 0xFFFFFFFF)
        outvl = { ktfdata.NUM, TimeFunc::UnixTime32ToInvString(ktfdata.Time), ktfdata.Ueff[0], ktfdata.Ueff[1],
            ktfdata.Ueff[2], ktfdata.Ieff[0], ktfdata.Ieff[1], ktfdata.Ieff[2], ktfdata.Frequency, ktfdata.U0,
            ktfdata.U1, ktfdata.U2, ktfdata.I0, ktfdata.I1, ktfdata.I2, ktfdata.Pf[0], ktfdata.Pf[1], ktfdata.Pf[2],
            ktfdata.Pf[3], ktfdata.Qf[0], ktfdata.Qf[1], ktfdata.Qf[2], ktfdata.Qf[3], ktfdata.Sf[0], ktfdata.Sf[1],
            ktfdata.Sf[2], ktfdata.Sf[3], ktfdata.Cosphi, ktfdata.Tmk, ktfdata.Tamb, ktfdata.Twin };
}

int JournKTF::measureSize()
{
    return sizeof(AVM_KTF::MeasureStruct);
}

int JournKTF::workJournalID()
{
    return AVM_KTF::workJourId;
}
