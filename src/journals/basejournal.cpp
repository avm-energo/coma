#include "basejournal.h"

#include "../widgets/wd_func.h"

#include <gen/timefunc.h>

BaseJournal::BaseJournal(QObject *parent)
    : QObject(parent)
    , timezone(TimeFunc::userTimeZone())
    , dataModel(new EDynamicTableModel(this))
    , proxyModel(new QSortFilterProxyModel(this))
{
}

const QStringList BaseJournal::jourListToStringList(const QList<ModuleTypes::Journal> &jourList) const
{
    QStringList retVal;
    retVal.reserve(jourList.size());
    for (auto &journal : jourList)
        retVal.push_back(journal.desc);
    return retVal;
}

void BaseJournal::setUserTimezone(QStringList &data)
{
    auto time_pos = std::find_if(data.begin(), data.end(), [](const QString &str) { return str.contains("UTC"); });
    if (time_pos != data.end())
        time_pos->replace("UTC", TimeFunc::userTimeZoneName());
}

UniquePointer<ETableView> BaseJournal::createModelView(QWidget *parent)
{
    auto modelView = WDFunc::NewTV(parent, viewName, proxyModel.get());
    return UniquePointer<ETableView>(modelView);
}

const QString &BaseJournal::getName() const
{
    return jourName;
}

void BaseJournal::fill(const QVariant &data)
{
    if (data.canConvert<DataTypes::FileStruct>())
    {
        if (!dataModel->isEmpty())
            dataModel->clearModel();
        auto fs = data.value<DataTypes::FileStruct>();
        dataModel->setHorizontalHeaderLabels(headers);
        fillTable(fs.data);
    }
    proxyModel->setSourceModel(dataModel.get());
    emit done("Прочитано успешно");
}

void BaseJournal::save(const QString &filename)
{
    Q_UNUSED(filename);
}
