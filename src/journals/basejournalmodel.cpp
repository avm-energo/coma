#include "basejournalmodel.h"

BaseJournalModel::BaseJournalModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

const QStringList BaseJournalModel::jourListToStrList(const QList<ModuleTypes::Journal> &jlist)
{
    QStringList retVal = {};
    for (auto &journal : jlist)
        retVal.push_back(journal.desc);
    return retVal;
}
