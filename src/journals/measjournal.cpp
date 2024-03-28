#include "measjournal.h"

namespace journals
{

MeasJournal::MeasJournal(const MeasSettings &settings, QObject *parent)
    : BaseJournal(parent), m_settings(settings), m_parser(new MeasParser(m_timezone, this))
{
    using namespace Device;
    std::sort(this->m_settings.begin(), this->m_settings.end(),
        [](const XmlDataTypes::MeasJournal &lhs, const XmlDataTypes::MeasJournal &rhs) { //
            return lhs.index < rhs.index;
        });
    m_viewName = "meas";
    m_modelHeaders = getHeaders();
    setUserTimezone(m_modelHeaders);
}

QStringList MeasJournal::getHeaders()
{
    QStringList retVal;
    retVal.reserve(m_settings.size());
    for (const auto &record : qAsConst(m_settings))
    {
        if (record.visibility)
            retVal.append(record.header);
    }
    return retVal;
}

void MeasJournal::fillModel(const QByteArray &ba)
{
    m_parser->update(ba, m_settings);
    const auto data = m_parser->parse(m_settings);
    m_dataModel->fillModel(data);
}

}
