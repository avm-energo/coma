#include "measjournal.h"

namespace journals
{

MeasJournal::MeasJournal(const MeasSettings &settings, QObject *parent)
    : BaseJournal(parent), settings(settings), parser(new MeasParser(timezone, this))
{
    std::sort(this->settings.begin(), this->settings.end(),
        [](const ModuleTypes::MeasJournal &lhs, const ModuleTypes::MeasJournal &rhs) { //
            return lhs.index < rhs.index;
        });
    jourName = "Журнал измерений";
    viewName = "meas";
    headers = getHeaders();
    type = JournalType::Meas;
    setUserTimezone(headers);
}

QStringList MeasJournal::getHeaders()
{
    QStringList retVal;
    retVal.reserve(settings.size());
    for (const auto &record : qAsConst(settings))
    {
        if (record.visibility)
            retVal.append(record.header);
    }
    return retVal;
}

void MeasJournal::fillModel(const QByteArray &ba)
{
    parser->update(ba, settings);
    const auto data = parser->parse(settings);
    dataModel->fillModel(data);
}

}
