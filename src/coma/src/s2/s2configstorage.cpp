#include "s2/s2configstorage.h"

#include "gen/floats.h"
#include "gen/std_ext.h"
#include <gen/strings.h>

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

config::itemVariant ConfigStorage::mergeWidgets(
    const config::itemVariant &oldWidget, const config::itemVariant &newWidget)
{
    enum Cases
    {
        NewWidgetIsTheSame,
        NewWidgetIsNotTheSame,
        NotProcessed
    };

    Cases flag = NotProcessed;
    config::itemVariant outw = newWidget;

    // first: check newWidget type
    std::visit(overloaded { [&](const auto &neww, const auto &oldw, auto &outw)
                   {
                       if ((neww.type == 0) || (neww.type == oldw.type))
                       {
                           outw.type = oldw.type;
                           flag = NewWidgetIsTheSame;
                       }
                       else
                           flag = NewWidgetIsNotTheSame;
                   } },
        newWidget, oldWidget, outw);

    if (flag == NewWidgetIsTheSame)
    {
        // if 0 or equal then it's a modification of the existent widget
        //      get old widget type
        //      process it depending of what it's type is (modify fields that are given)
        std::visit(
            overloaded {
                [&](const delegate::DoubleSpinBoxGroup &oldw, delegate::DoubleSpinBoxGroup &outw)
                {
                    if (outw.decimals == U32MAX)
                        outw.decimals = oldw.decimals;
                    if (outw.min == F4MAX)
                        outw.min = oldw.min;
                    if (outw.max == F4MAX)
                        outw.max = oldw.max;
                    if (outw.count == U32MAX)
                        outw.count = oldw.count;
                },
                [&](const delegate::DoubleSpinBoxWidget &oldw, delegate::DoubleSpinBoxWidget &outw)
                {
                    if (outw.decimals == U32MAX)
                        outw.decimals = oldw.decimals;
                    if (outw.min == F4MAX)
                        outw.min = oldw.min;
                    if (outw.max == F4MAX)
                        outw.max = oldw.max;
                },
                [&](const delegate::CheckBoxGroup &oldw, delegate::CheckBoxGroup &outw)
                {
                    if (outw.items == STRLISTINF)
                        outw.items = oldw.items;
                    if (outw.count == U32MAX)
                        outw.count = oldw.count;
                },
                [&](const delegate::ComboBox &oldw, delegate::ComboBox &outw)
                {
                    if (outw.model == STRLISTINF)
                        outw.model = oldw.model;
                    if (outw.primaryField == delegate::ComboBox::unknown)
                        outw.primaryField = oldw.primaryField;
                },
                [&](const delegate::ComboBoxGroup &oldw, delegate::ComboBoxGroup &outw)
                {
                    if (outw.model == STRLISTINF)
                        outw.model = oldw.model;
                    if (outw.primaryField == delegate::ComboBox::unknown)
                        outw.primaryField = oldw.primaryField;
                    if (outw.count == U32MAX)
                        outw.count = oldw.count;
                    if (outw.items == STRLISTINF)
                        outw.items = oldw.items;
                },
                [&](const auto &oldw, auto &outw)
                { qDebug() << "Type fields of widgets are equal but widgets itself are not equal!"; },
            },
            oldWidget, outw);
    }
    // process all shared fields and set them to new widget's ones if given
    std::visit(overloaded { [&](const auto &oldw, auto &outw)
                   {
                       if (outw.desc == STRINF)
                           outw.desc = oldw.desc;
                       if (outw.group == U16MAX)
                           outw.group = oldw.group;
                       if (outw.toolTip == STRINF)
                           outw.toolTip = oldw.toolTip;
                   } },
        oldWidget, outw);
    return outw;
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
    {
        auto w = m_widgetMap.find(id);
        if (w != m_widgetMap.end())
            m_widgetMap[id] = mergeWidgets(w.value(), widget); // merging widget
        else
            m_widgetMap[id] = widget;
    }
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
