#ifndef SYSTEMEVENTSJOURNALMODEL_H
#define SYSTEMEVENTSJOURNALMODEL_H

#include "baseeventsjournalmodel.h"

#include <QObject>

class SystemEventsJournalModel : public BaseEventsJournalModel
{
    Q_OBJECT
public:
    SystemEventsJournalModel(QObject *parent = nullptr);
};

#endif // SYSTEMEVENTSJOURNALMODEL_H
