#include "kivmeasjournalmodel.h"

KIVMeasJournalModel::KIVMeasJournalModel(const QList<ModuleTypes::Journal> &headers, QObject *parent)
    : MeasJournalModel(headers, parent)
{
}
