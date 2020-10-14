#ifndef JOURNKTF_H
#define JOURNKTF_H

#include "journals.h"
#include <QObject>

#define KTFWORKJOURNID 5011

class JournKTF : public Journals
{
    Q_OBJECT
public:
    struct MeasureStruct
    {
        quint32 NUM;
        quint32 Time;
        float Ueff[3];
        float Ieff[3];
        float Frequency;
        float U0;
        float U1;
        float U2;
        float I0;
        float I1;
        float I2;
        float Pf[4];
        float Qf[4];
        float Sf[4];
        float Cosphi;
        float Twin;
        float Tmk;
        float Tamb;
        quint32 res;
    };

    JournKTF(QObject *parent = nullptr);
    void setWorkJourDescription() override;
    void setMeasJourHeaders() override;
    void setMeasRecord(char *file, QVector<QVariant> &outvl) override;
    int measureSize() override;
    int workJournalID() override;
};

#endif // JOURNKTF_H
