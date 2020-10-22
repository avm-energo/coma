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
        float U0;
        float U1;
        float U2;
        float I0;
        float I1;
        float I2;
        float Cbush[3];
        float Tg_d[3];
        float dCbush[3];
        float dTg_d[3];
        float Iunb;
        float Phy_unb;
        float Tmk;
        float Tamb;
        quint32 res;
    };

    JournKDV(QObject *parent = nullptr);
    void setWorkJourDescription() override;
    void setMeasJourHeaders() override;
    void setMeasRecord(char *file, QVector<QVariant> &outvl) override;
    int measureSize() override;
    int workJournalID() override;
};

#endif // JOURNKDV_H