#ifndef JOURNKTF_H
#define JOURNKTF_H

#include "journals.h"

class JournKTF : public Journals
{
    Q_OBJECT
public:
    JournKTF(const ModuleTypes::JourMap &jourMap, QObject *parent = nullptr);
    QVector<QVariant> createMeasRecord(const char *file) override;
    virtual QVector<QVector<QVariant>> createMeas(const QByteArray &array) override;
    int measureSize() override;
};

#endif // JOURNKTF_H
