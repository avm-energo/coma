#include "workjournal.h"

namespace journals
{

WorkJournal::WorkJournal(const QMap<quint32, QString> &desc, QObject *parent)
    : BaseJournal(parent), desriptions(desc), parser(new EventParser(this))
{
    jourName = "Рабочий журнал";
    viewName = "work";
    headers = BaseJournal::eventJourHeaders;
    type = JournalType::Work;
    setUserTimezone(headers);
}

void WorkJournal::fillModel(const QByteArray &ba)
{
    parser->update(ba);
    const auto data = parser->parse(desriptions, timezone);
    dataModel->fillModel(data);
}

}
