#pragma once

#include "../models/edynamictablemodel.h"
#include "../module/modulesettings.h"
#include "../widgets/etableview.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>

class BaseJournal : QObject
{
    Q_OBJECT
private:
    QString jourName;
    QString viewName;

    const QStringList jourListToStringList(const QList<ModuleTypes::Journal> &jourList);
    void setupJournal(Modules::JournalType jourType);

protected:
    QTimeZone timezone;
    QStringList headers;
    UniquePointer<EDynamicTableModel> dataModel;
    UniquePointer<QSortFilterProxyModel> proxyModel;

public:
    BaseJournal(Modules::JournalType jourType, QList<ModuleTypes::Journal> &jourList, QObject *parent = nullptr);
    UniquePointer<ETableView> createModelView(QWidget *parent);
    const QString &getJournalName() const;
};
