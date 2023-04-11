#include "measjournalmodel.h"

#include <gen/timefunc.h>

MeasJournalModel::MeasJournalModel(const QList<ModuleTypes::Journal> &headers, QObject *parent)
    : BaseJournalModel(parent)
{
    _headers = jourListToStrList(headers);
    auto time_pos
        = std::find_if(_headers.begin(), _headers.end(), [](const QString &str) { return str.contains("UTC"); });
    if (time_pos != _headers.end())
        time_pos->replace("UTC", TimeFunc::userTimeZoneName());
}
