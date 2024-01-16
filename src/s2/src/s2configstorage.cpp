#include "s2/s2configstorage.h"

#include <QDebug>

namespace S2
{

ConfigStorage::ConfigStorage(token token, QObject *parent) : QObject(parent), m_status(ParseStatus::NotYetParsed)
{
    Q_UNUSED(token);
}

ParseStatus ConfigStorage::getParseStatus() const
{
    return m_status;
}

void ConfigStorage::setParseStatus(const ParseStatus pStatus)
{
    m_status = pStatus;
}

void ConfigStorage::clearDetailData() noexcept
{
    m_widgetDetailMap.clear();
}

const std::map<QString, quint32> &ConfigStorage::getIdByNameMap() const
{
    return m_idByName;
}

const std::map<quint32, ctti::unnamed_type_id_t> &ConfigStorage::getTypeByIdMap() const
{
    return m_typeById;
}

const std::map<quint32, QString> &ConfigStorage::getConfigTabs() const
{
    return m_configTabs;
}

const std::map<quint32, WidgetDetail> &ConfigStorage::getWidgetDetailMap() const
{
    return m_widgetDetailMap;
}

void ConfigStorage::nameDataReceive(const quint32 id, const QString &name)
{
    if (id == 0)
        qWarning() << "Invalid S2 config id: " << id;
    else if (name.isEmpty())
        qWarning() << "Empty S2 name for item with id: " << id;
    else
        m_idByName.insert({ name, id });
}

const config::widgetMap &ConfigStorage::getWidgetMap() const
{
    return m_widgetMap;
}

void ConfigStorage::typeDataReceive(const quint32 id, const std::uint64_t typeId)
{
    if (id == 0)
        qWarning() << "Invalid S2 config id: " << id;
    else if (typeId != 0)
        m_typeById.insert({ id, typeId });
}

void ConfigStorage::widgetDataReceive(const quint32 id, const config::itemVariant &widget)
{
    if (id == 0)
        qWarning() << "Invalid S2 widget id: " << id;
    else if (widget.valueless_by_exception())
        qWarning() << "Invalid S2 widget data, widget id: " << id;
    else
        m_widgetMap.insert({ id, widget });
}

void ConfigStorage::configTabDataReceive(const quint32 id, const QString &tabName)
{
    if (tabName.isEmpty())
        qWarning() << "Empty tab name, tab id: " << id;
    else
        m_configTabs.insert({ id, tabName });
}

void ConfigStorage::widgetDetailsReceive(const quint32 id, const bool visib, const quint16 count)
{
    m_widgetDetailMap.insert({ id, { visib, count } });
}

} // namespace S2
