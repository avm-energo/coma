#ifndef JOURNKTF_H
#define JOURNKTF_H

#include "journals.h"

class JournKTF : public Journals
{
    Q_OBJECT
public:
    JournKTF(QObject *parent = nullptr);
    void setWorkJourDescription() override;
    void setMeasJourHeaders() override;
    QVector<QVariant> createMeasRecord(const char *file) override;
    int measureSize() override;
    int workJournalID() override;
};

#endif // JOURNKTF_H
