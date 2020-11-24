#ifndef JOURNKDV_H
#define JOURNKDV_H

#include "journals.h"

class JournKDV : public Journals
{
    Q_OBJECT
public:
    JournKDV(QObject *parent = nullptr);
    void setWorkJourDescription() override;
    void setMeasJourHeaders() override;
    void setMeasRecord(char *file, QVector<QVariant> &outvl) override;
    int measureSize() override;
    int workJournalID() override;
};

#endif // JOURNKDV_H
