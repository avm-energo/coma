#ifndef JOURNKIV_H
#define JOURNKIV_H

#include "journals.h"

class JournKIV : public Journals
{
    Q_OBJECT
public:
    JournKIV(const ModuleTypes::JourMap &jourMap, QObject *parent = nullptr);
    QVector<QVariant> createMeasRecord(const char *file) override;
    virtual QVector<QVector<QVariant>> createMeas(const QByteArray &array) override;
    int measureSize() override;
};

#endif // JOURNKIV_H
