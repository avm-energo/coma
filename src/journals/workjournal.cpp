#include "workjournal.h"

namespace journals
{

WorkJournal::WorkJournal(const QMap<quint32, QString> &desc, QObject *parent)
    : BaseJournal(parent), m_desriptions(desc), m_parser(new EventParser(this))
{
    m_viewName = "work";
    m_modelHeaders = BaseJournal::s_eventJourHeaders;
    setUserTimezone(m_modelHeaders);
}

void WorkJournal::fillModel(const QByteArray &ba)
{
    m_parser->update(ba);
    const auto data = m_parser->parse(m_desriptions, m_timezone);
    m_dataModel->fillModel(data);
}

}
