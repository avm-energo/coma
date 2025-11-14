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
    config::itemVariant outw;

    // first: check newWidget type
    std::visit(overloaded { [&](const auto &neww, const auto &oldw)
                   {
                       if ((neww.type == 0) || (neww.type == oldw.type))
                           flag = NewWidgetIsTheSame;
                       else
                           flag = NewWidgetIsNotTheSame;
                   } },
        newWidget, oldWidget);
    if (flag == NewWidgetIsTheSame)
    {
        // if 0 or equal then it's a modification of the existent widget
        //      get old widget type
        //      process it depending of what it's type is (modify fields that are given)
        /*      std::visit(
                  overloaded {
                      [&](const delegate::DoubleSpinBoxGroup &oldw, const auto &neww)
                      {
                          delegate::DoubleSpinBoxGroup outw2 = oldw;
                          if (neww.decimals != U32MAX)
                              outw2.decimals = neww.decimals;
                          if (neww.min != F4MAX)
                              outw2.min = neww.min;
                          if (neww.max != F4MAX)
                              outw2.max = neww.max;
                          if (neww.count != U32MAX)
                              outw2.count = neww.count;
                          outw = outw2;
                      },
                      [&](const delegate::DoubleSpinBoxWidget &neww, const delegate::DoubleSpinBoxWidget &oldw)
                      {
                          delegate::DoubleSpinBoxWidget outw2 = oldw;
                          if (neww.decimals != U32MAX)
                              outw2.decimals = neww.decimals;
                          if (neww.min != F4MAX)
                              outw2.min = neww.min;
                          if (neww.max != F4MAX)
                              outw2.max = neww.max;
                          outw = outw2;
                      },
                      [&](const delegate::CheckBoxGroup &neww, const delegate::CheckBoxGroup &oldw)
                      {
                          delegate::CheckBoxGroup outw2 = oldw;
                          if (neww.items != STRLISTINF)
                              outw2.items = neww.items;
                          if (neww.count != U32MAX)
                              outw2.count = neww.count;
                          outw = outw2;
                      },
                      [&](const delegate::ComboBox &neww, const delegate::ComboBox &oldw)
                      {
                          delegate::ComboBox outw2 = oldw;
                          if (neww.model != STRLISTINF)
                              outw2.model = neww.model;
                          if (neww.primaryField != delegate::ComboBox::unknown)
                              outw2.primaryField = neww.primaryField;
                          outw = outw2;
                      },
                      [&](const delegate::ComboBoxGroup &neww, const delegate::ComboBoxGroup &oldw)
                      {
                          delegate::ComboBoxGroup outw2 = oldw;
                          if (neww.model != STRLISTINF)
                              outw2.model = neww.model;
                          if (neww.primaryField != delegate::ComboBox::unknown)
                              outw2.primaryField = neww.primaryField;
                          if (neww.count != U32MAX)
                              outw2.count = neww.count;
                          if (neww.items != STRLISTINF)
                              outw2.items = neww.items;
                          outw = outw2;
                      },
                      [&](const auto &neww, const auto &oldw) { outw = neww; },
                  },
                  oldWidget, newWidget); */
    }
    // if not
    //      get all the fields from new widget except shared ones
    // process all shared fields and set them to new widget's ones if given
    std::visit(
        overloaded {
            [&](const delegate::DoubleSpinBoxGroup &neww, const delegate::DoubleSpinBoxGroup &oldw)
            {
                delegate::DoubleSpinBoxGroup outw2 = oldw;
                if (neww.decimals != U32MAX)
                    outw2.decimals = neww.decimals;
                if (neww.min != F4MAX)
                    outw2.min = neww.min;
                if (neww.max != F4MAX)
                    outw2.max = neww.max;
                if (neww.count != U32MAX)
                    outw2.count = neww.count;
                outw = outw2;
            },
            [&](const delegate::DoubleSpinBoxWidget &neww, const delegate::DoubleSpinBoxWidget &oldw)
            {
                delegate::DoubleSpinBoxWidget outw2 = oldw;
                if (neww.decimals != U32MAX)
                    outw2.decimals = neww.decimals;
                if (neww.min != F4MAX)
                    outw2.min = neww.min;
                if (neww.max != F4MAX)
                    outw2.max = neww.max;
                outw = outw2;
            },
            [&](const delegate::CheckBoxGroup &neww, const delegate::CheckBoxGroup &oldw)
            {
                delegate::CheckBoxGroup outw2 = oldw;
                if (neww.items != STRLISTINF)
                    outw2.items = neww.items;
                if (neww.count != U32MAX)
                    outw2.count = neww.count;
                outw = outw2;
            },
            [&](const delegate::ComboBox &neww, const delegate::ComboBox &oldw)
            {
                delegate::ComboBox outw2 = oldw;
                if (neww.model != STRLISTINF)
                    outw2.model = neww.model;
                if (neww.primaryField != delegate::ComboBox::unknown)
                    outw2.primaryField = neww.primaryField;
                outw = outw2;
            },
            [&](const delegate::ComboBoxGroup &neww, const delegate::ComboBoxGroup &oldw)
            {
                delegate::ComboBoxGroup outw2 = oldw;
                if (neww.model != STRLISTINF)
                    outw2.model = neww.model;
                if (neww.primaryField != delegate::ComboBox::unknown)
                    outw2.primaryField = neww.primaryField;
                if (neww.count != U32MAX)
                    outw2.count = neww.count;
                if (neww.items != STRLISTINF)
                    outw2.items = neww.items;
                outw = outw2;
            },
            [&](const auto &neww, const auto &oldw) { outw = neww; },
        },
        newWidget, oldWidget);
    auto mainEqualizer = [&](auto &neww, auto &oldw) {

    };
    std::visit(overloaded { [&](const auto &neww, const auto &oldw, auto &outw)
                   {
                       outw.desc = (neww.desc == STRINF) ? oldw.desc : neww.desc;
                       outw.group = (neww.group == U16MAX) ? oldw.group : neww.group;
                       outw.toolTip = (neww.toolTip == STRINF) ? oldw.desc : neww.desc;
                       outw.type = (neww.type == 0) ? oldw.type : neww.type;
                   } },
        newWidget, oldWidget, outw);
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
