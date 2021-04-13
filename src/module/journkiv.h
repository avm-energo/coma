#ifndef JOURNKIV_H
#define JOURNKIV_H

#include "journals.h"

class JournKIV : public Journals
{
    Q_OBJECT
public:
    JournKIV(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent = nullptr);

    QVector<QVariant> createMeasRecord(const char *file) override;
    int measureSize() override;
};

#endif // JOURNKIV_H
