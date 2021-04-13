#ifndef JOURNKDV_H
#define JOURNKDV_H

#include "journals.h"

class JournKDV : public Journals
{
    Q_OBJECT
public:
    JournKDV(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent = nullptr);

    QVector<QVariant> createMeasRecord(const char *file) override;
    int measureSize() override;
};

#endif // JOURNKDV_H
