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

Q_ENUM_NS(ItemType)
struct Widget
{
    Widget(const ctti::unnamed_type_id_t type_) : type(type_)
    {
    }
    Widget(const ctti::unnamed_type_id_t type_, const QString &desc_) : type(type_), desc(desc_)
    {
    }
    ctti::unnamed_type_id_t type;
    QString desc;
};

struct DoubleSpinBoxWidget : Widget
{
    DoubleSpinBoxWidget(const ctti::unnamed_type_id_t type_) : Widget(type_)
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
    DoubleSpinBoxGroup(const ctti::unnamed_type_id_t type_) : DoubleSpinBoxWidget(type_)
    {
    }
};
struct CheckBoxGroup : Widget, Group
{
    CheckBoxGroup(const ctti::unnamed_type_id_t type_) : Widget(type_)
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
    QComboBox(const ctti::unnamed_type_id_t type_) : Widget(type_)
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
    ItemType itemType;
    BciNumber parent;
};

using itemVariant = std::variant<Widget, QComboBox, DoubleSpinBoxGroup, DoubleSpinBoxWidget, CheckBoxGroup, Item>;
}
using widgetMap = std::map<BciNumber, delegate::itemVariant>;
