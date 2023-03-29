#ifndef KIVMEASJOURNALMODEL_H
#define KIVMEASJOURNALMODEL_H

#include "measjournalmodel.h"

#include <QObject>

class KIVMeasJournalModel : public MeasJournalModel
{
    Q_OBJECT
public:
    KIVMeasJournalModel(const QList<ModuleTypes::Journal> &headers, QObject *parent = nullptr);
};

#endif // KIVMEASJOURNALMODEL_H
