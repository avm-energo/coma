#ifndef JOURNKIV_H
#define JOURNKIV_H

#include "journals.h"

class JournKIV : public Journals
{
    Q_OBJECT
public:
    JournKIV(QObject *parent = nullptr);
    void setWorkJourDescription() override;
    void setMeasJourHeaders() override;
    QVector<QVariant> createMeasRecord(const char *file) override;
    int measureSize() override;
    int workJournalID() override;
};

#endif // JOURNKIV_H
