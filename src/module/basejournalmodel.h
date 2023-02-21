#ifndef BASEJOURNALMODEL_H
#define BASEJOURNALMODEL_H

#include "modulesettings.h"

#include <QObject>
#include <QSortFilterProxyModel>

class BaseJournalModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    BaseJournalModel(QObject *parent = nullptr);

    const QStringList jourListToStrList(const QList<ModuleTypes::Journal> &jlist);

protected:
    QStringList _headers;
};

#endif // BASEJOURNALMODEL_H
