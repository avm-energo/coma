#include "s2/s2configstorage.h"

#include <QDebug>

namespace S2
{

ConfigStorage::ConfigStorage(QObject *parent) : QObject(parent) { }

void ConfigStorage::clearDetailData() noexcept
{
    m_widgetDetailMap.clear();
}

const QMap<QString, u32> &ConfigStorage::getIdByNameMap() const
{
    return m_idByName;
}

const QMap<u32, ctti::detail::hash_t> &ConfigStorage::getTypeByIdMap() const
{
    return m_typeById;
}

bool ConfigStorage::getDType(u32 id)
{
    auto search = m_dtypes.find(id);
    if (search == m_dtypes.end())
        return false;
    return search.value();
}

const QMap<u32, QString> &ConfigStorage::getConfigTabs() const
{
    return m_configTabs;
}

void ConfigStorage::nameDataReceive(const u32 id, const QString &name)
{
    if (id == 0)
        qWarning() << "Invalid S2 config id: " << id;
    else if (name.isEmpty())
        qWarning() << "Empty S2 name for item with id: " << id;
    else
    {
        auto key = m_idByName.key(id);
        m_idByName.remove(key);
        m_idByName[name] = id;
    }
}

const QMap<u32, ConfigStorage::WidgetDetail> &ConfigStorage::getWidgetDetailMap() const
{
    return m_widgetDetailMap;
}

u32 ConfigStorage::getIdFor(const QString &name) const noexcept
{
    auto search = m_idByName.find(name);
    if (search == m_idByName.cend())
        return 0;
    else
        return search.value();
}

const config::widgetMap &ConfigStorage::getWidgetMap() const
{
    return m_widgetMap;
}

void ConfigStorage::typeDataReceive(const u32 id, const ctti::detail::hash_t typeId)
{
    if (id == 0)
        qWarning() << "Invalid S2 config id: " << id;
    else if (typeId != 0)
        m_typeById[id] = typeId;
}

void ConfigStorage::dtypeDataReceive(const u32 id, bool dtype)
{
    if (id == 0)
        qWarning() << "Invalid S2 config id: " << id;
    m_dtypes[id] = dtype;
}

void ConfigStorage::widgetDataReceive(const u32 id, const config::itemVariant &widget)
{
    if (id == 0)
        qWarning() << "Invalid S2 widget id: " << id;
    else if (widget.valueless_by_exception())
        qWarning() << "Invalid S2 widget data, widget id: " << id;
    else
        m_widgetMap[id] = widget;
}

void ConfigStorage::configTabDataReceive(const u32 id, const QString &tabName)
{
    if (tabName.isEmpty())
        qWarning() << "Empty tab name, tab id: " << id;
    else
        m_configTabs[id] = tabName;
}

void ConfigStorage::widgetDetailsReceive(const u32 id, const bool visib, const quint16 count, const quint16 order)
{
    m_widgetDetailMap[id] = { visib, count, order };
}

} // namespace S2
