#ifndef JOURNKDV_H
#define JOURNKDV_H

#include "journals.h"

class JournKDV : public Journals
{
    Q_OBJECT
public:
    JournKDV(const ModuleTypes::JourMap &jourMap, QObject *parent = nullptr);
    QVector<QVariant> createMeasRecord(const char *file) override;
    virtual QVector<QVector<QVariant>> createMeas(const QByteArray &array) override;
    int measureSize() override;
};

#endif // JOURNKDV_H
