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
    void setMeasRecord(char *file, QVector<QVariant> &outvl) override;
    int measureSize() override;
    int workJournalID() override;
};

#endif // JOURNKTF_H
