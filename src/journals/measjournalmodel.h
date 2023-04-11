#ifndef MEASJOURNALMODEL_H
#define MEASJOURNALMODEL_H

#include "basejournalmodel.h"
//#include "modulesettings.h"

#include <QObject>

class MeasJournalModel : public BaseJournalModel
{
    Q_OBJECT
public:
    MeasJournalModel(const QList<ModuleTypes::Journal> &headers, QObject *parent = nullptr);

    virtual QVector<QVariant> createMeasRecord(const char *file) = 0;
    virtual QVector<QVector<QVariant>> createMeas(const QByteArray &array) = 0;
};

#endif // MEASJOURNALMODEL_H
