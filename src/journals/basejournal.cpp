#include "basejournal.h"

#include "../widgets/wd_func.h"

BaseJournal::BaseJournal(Modules::JournalType jourType, QList<ModuleTypes::Journal> &jourList, QObject *parent)
    : QObject(parent)
    , timezone(TimeFunc::userTimeZone())
    , headers {}
    , dataModel(new EDynamicTableModel(this))
    , proxyModel(new QSortFilterProxyModel(this))
{
    setupJournal(jourType);
}

void BaseJournal::setupJournal(Modules::JournalType jourType)
{
    switch (jourType)
    {
    case Modules::JournalType::System:
        jourName = "системный журнал";
        viewName = "system";
        break;
    case Modules::JournalType::Work:
        jourName = "рабочий журнал";
        viewName = "work";
        break;
    case Modules::JournalType::Meas:
        jourName = "журнал измерений";
        viewName = "meas";
        break;
    default:
        qCritical() << "Unknown journal type: " << jourType;
        break;
    }
}

const QStringList BaseJournal::jourListToStringList(const QList<ModuleTypes::Journal> &jourList)
{
    QStringList retVal;
    retVal.reserve(jourList.size());
    for (auto &journal : jourList)
        retVal.push_back(journal.desc);
    return retVal;
}

UniquePointer<ETableView> BaseJournal::createModelView(QWidget *parent)
{
    auto modelView = WDFunc::NewTV(parent, viewName, proxyModel.get());
    return UniquePointer<ETableView>(modelView);
}

const QString &BaseJournal::getJournalName() const
{
    return jourName;
}
