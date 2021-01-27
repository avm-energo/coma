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
    QVector<QVariant> createMeasRecord(const char *file) override;
    int measureSize() override;
    int workJournalID() override;
};

#endif // JOURNKDV_H
