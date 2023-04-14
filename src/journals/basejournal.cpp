#include "basejournal.h"

#include "../widgets/wd_func.h"

#include <gen/timefunc.h>

namespace journals
{

BaseJournal::BaseJournal(QObject *parent)
    : QObject(parent)
    , timezone(TimeFunc::userTimeZone())
    , dataModel(new EDynamicTableModel(this))
    , proxyModel(new QSortFilterProxyModel(this))
{
}

void BaseJournal::setUserTimezone(QStringList &data)
{
    auto time_pos = std::find_if(data.begin(), data.end(), [](const QString &str) { return str.contains("UTC"); });
    if (time_pos != data.end())
        time_pos->replace("UTC", TimeFunc::userTimeZoneName());
}

ETableView *BaseJournal::createModelView(QWidget *parent) const
{
    auto modelView = WDFunc::NewTV(parent, viewName, proxyModel.get());
    return modelView;
}

const QString &BaseJournal::getName() const
{
    return jourName;
}

const QString &BaseJournal::getViewName() const
{
    return viewName;
}

const JournalType BaseJournal::getType() const
{
    return type;
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

}
