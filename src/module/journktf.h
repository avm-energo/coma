#ifndef JOURNKTF_H
#define JOURNKTF_H

#include "journals.h"

class JournKTF : public Journals
{
    Q_OBJECT
public:
    JournKTF(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent = nullptr);

    QVector<QVariant> createMeasRecord(const char *file) override;
    int measureSize() override;
};

#endif // JOURNKTF_H
