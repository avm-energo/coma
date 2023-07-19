#include "s2configstorage.h"

#include <QDebug>

namespace S2
{

ConfigStorage::ConfigStorage(token token, QObject *parent) : QObject(parent), status(ParseStatus::NotYetParsed)
{
    Q_UNUSED(token);
}

ParseStatus ConfigStorage::getParseStatus() const
{
    return status;
}

void ConfigStorage::setParseStatus(const ParseStatus pStatus)
{
    status = pStatus;
}

void ConfigStorage::clearDetailData() noexcept
{
    widgetDetailMap.clear();
}

const std::map<QString, quint32> &ConfigStorage::getIdByNameMap() const
{
    return idByName;
}

const std::map<quint32, ctti::unnamed_type_id_t> &ConfigStorage::getTypeByIdMap() const
{
    return typeById;
}

const std::map<quint32, QString> &ConfigStorage::getConfigTabs() const
{
    return configTabs;
}

const std::map<quint32, WidgetDetail> &ConfigStorage::getWidgetDetailMap() const
{
    return widgetDetailMap;
}

void ConfigStorage::nameDataReceive(const quint32 id, const QString &name)
{
    if (id == 0)
        qWarning() << "Invalid S2 config id: " << id;
    else if (name.isEmpty())
        qWarning() << "Empty S2 name for item with id: " << id;
    else
        idByName.insert({ name, id });
}

const config::widgetMap &ConfigStorage::getWidgetMap() const
{
    return widgetMap;
}

void ConfigStorage::typeDataReceive(const quint32 id, const std::uint64_t typeId)
{
    if (id == 0)
        qWarning() << "Invalid S2 config id: " << id;
    else if (typeId != 0)
        typeById.insert({ id, typeId });
}

void ConfigStorage::widgetDataReceive(const quint32 id, const config::itemVariant &widget)
{
    if (id == 0)
        qWarning() << "Invalid S2 widget id: " << id;
    else if (widget.valueless_by_exception())
        qWarning() << "Invalid S2 widget data, widget id: " << id;
    else
        widgetMap.insert({ id, widget });
}

void ConfigStorage::configTabDataReceive(const quint32 id, const QString &tabName)
{
    if (tabName.isEmpty())
        qWarning() << "Empty tab name, tab id: " << id;
    else
        configTabs.insert({ id, tabName });
}

void ConfigStorage::widgetDetailsReceive(const quint32 id, const bool visib, const quint16 count)
{
    widgetDetailMap.insert({ id, { visib, count } });
}

}
