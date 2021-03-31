#pragma once
#include "../ctti/type_id.hpp"
#include "../module/modules.h"

#include <QObject>
#include <variant>
namespace delegate
{
Q_NAMESPACE

enum class ItemType : int
{
    ModbusItem
};

enum WidgetGroup : int
{
    EmptyGroup = 0,
    MainGroup = 1,
    ComGroup = 2,
    AnalogGroup = 3,
    ThresholdGroup = 4,
    HystGroup = 5,
    IntervalGroup,
    TempGroup = 7,
    ModbusGroup = 8,
    //...to be continued
};
Q_ENUM_NS(WidgetGroup)
Q_ENUM_NS(ItemType)
struct Widget
{
    Widget(const ctti::unnamed_type_id_t type_) : type(type_)
    {
    }
    Widget(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : type(type_), group(group_)
    {
    }
    Widget(const ctti::unnamed_type_id_t type_, const QString &desc_, const WidgetGroup group_)
        : type(type_), desc(desc_), group(group_)
    {
    }
    ctti::unnamed_type_id_t type;
    QString desc;
    WidgetGroup group;
};

struct DoubleSpinBoxWidget : Widget
{
    DoubleSpinBoxWidget(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
    double min;
    double max;
    int decimals;
};
struct Group
{
    int count;
    QStringList items;
};
struct DoubleSpinBoxGroup : DoubleSpinBoxWidget, Group
{
    DoubleSpinBoxGroup(const ctti::unnamed_type_id_t type_, const WidgetGroup group_)
        : DoubleSpinBoxWidget(type_, group_)
    {
    }
};
struct CheckBoxGroup : Widget, Group
{
    CheckBoxGroup(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
};

struct QComboBox : Widget, Group
{
    enum PrimaryField : bool
    {
        index = 0,
        data = 1
    };
    QComboBox(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : Widget(type_, group_)
    {
    }
    PrimaryField primaryField = index;
};
struct Item : Widget
{
    enum ModbusColumns : int
    {
        SensorType = 0,
        BaudRate,
        Parity,
        StopBits,
        Timeout,
        Address,
        FuncCode,
        DataType,
        Register
    };
    Item(const ctti::unnamed_type_id_t type_) : Widget(type_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_) : Widget(type_), itemType(itype_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_, const BciNumber parent_)
        : Widget(type_), itemType(itype_), parent(parent_)
    {
    }
    Item(const ctti::unnamed_type_id_t type_, const ItemType itype_, const BciNumber parent_, const WidgetGroup group_)
        : Widget(type_, group_), itemType(itype_), parent(parent_)
    {
    }
    ItemType itemType;
    BciNumber parent;
};

using itemVariant = std::variant<Widget, QComboBox, DoubleSpinBoxGroup, DoubleSpinBoxWidget, CheckBoxGroup, Item>;
}
using widgetMap = std::map<BciNumber, delegate::itemVariant>;
