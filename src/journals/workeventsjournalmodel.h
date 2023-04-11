#ifndef WORKEVENTSJOURNALMODEL_H
#define WORKEVENTSJOURNALMODEL_H

#include "baseeventsjournalmodel.h"

#include <QObject>

class WorkEventsJournalModel : public BaseEventsJournalModel
{
    Q_OBJECT
public:
    WorkEventsJournalModel(QObject *parent = nullptr);
};

#endif // WORKEVENTSJOURNALMODEL_H
