#ifndef JOURNKDV_H
#define JOURNKDV_H

#include "journals.h"
#include <QObject>

#define KDVWORKJOURNID 3010

class JournKDV : public Journals
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
        float U1;
        float U2;
        float I1;
        float I2;
        float Pf_sum;
        float Qf_sum;
        float Sf_sum;
        float CosPhi;
        float Tmk;
        float Tamb;
        float Twin;
        float VibrSCZ_V[6];
        float VibrSCZ_D[6];
    };

    JournKDV(QObject *parent = nullptr);
    void setWorkJourDescription() override;
    void setMeasJourHeaders() override;
    void setMeasRecord(char *file, QVector<QVariant> &outvl) override;
    int measureSize() override;
    int workJournalID() override;
};

#endif // JOURNKDV_H
