#include "workjournal.h"

#include "../models/edynamictablemodel.h"

#include <QSortFilterProxyModel>

namespace journals
{

WorkJournal::WorkJournal(const QMap<quint32, QString> &desc, QObject *parent)
    : BaseJournal(parent), desriptions(desc), parser(new EventParser(this))
{
    viewName = "work";
    headers = BaseJournal::s_eventJourHeaders;
    setUserTimezone(headers);
}

void WorkJournal::fillModel(const QByteArray &ba)
{
    parser->update(ba);
    const auto data = parser->parse(desriptions, timezone);
    dataModel->fillModel(data);
}

}
