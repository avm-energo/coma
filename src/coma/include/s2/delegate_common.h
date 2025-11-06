#pragma once

#include <ctti/type_id.hpp>
#include <gen/integers.h>

#include <QMap>
#include <QString>
#include <variant>

namespace delegate
{
// TODO: так и должно быть?
enum class ItemType : int
{
    ModbusItem
};

struct Widget
{
    Widget() : type(0) { }
    Widget(const ctti::unnamed_type_id_t type_) : type(type_) { }

    Widget(const ctti::unnamed_type_id_t type_, const u16 group_) : type(type_), group(group_) { }

    Widget(const ctti::unnamed_type_id_t type_, const QString &desc_, const u16 &group_, const QString &toolTip_)
        : type(type_)
        , desc(desc_)
        , group(group_)
        , toolTip(toolTip_)
    {
    }

    ctti::unnamed_type_id_t type;
    QString desc;
    u16 group;
    QString toolTip;
};

struct DoubleSpinBoxWidget : Widget
{
    DoubleSpinBoxWidget() { }
    DoubleSpinBoxWidget(const ctti::unnamed_type_id_t &type_, const QString &desc_, //
        const u16 &group_, const QString &toolTip_)                                 //
        : Widget(type_, desc_, group_, toolTip_)                                    //
    {
    }

    double min;
    double max;
    quint32 decimals;
};

struct Group
{
    quint32 count;
    QStringList items;
};

struct DoubleSpinBoxGroup : DoubleSpinBoxWidget, Group
{
    DoubleSpinBoxGroup() { }
    DoubleSpinBoxGroup(
        const ctti::unnamed_type_id_t &type_, const QString &desc_, const u16 &group_, const QString &toolTip_)
        : DoubleSpinBoxWidget(type_, desc_, group_, toolTip_)
    {
    }
};

struct CheckBoxGroup : Widget, Group
{
    CheckBoxGroup() { }
    CheckBoxGroup(
        const ctti::unnamed_type_id_t &type_, const QString &desc_, const u16 &group_, const QString &toolTip_)
        : Widget(type_, desc_, group_, toolTip_)
    {
    }
};

struct ComboBox : Widget
{
    enum PrimaryField : int
    {
        index = 0,
        data = 1,
        bitfield = 2
    };

    ComboBox() { }
    ComboBox(const ctti::unnamed_type_id_t &type_, const QString &desc_, const u16 &group_, const QString &toolTip_)
        : Widget(type_, desc_, group_, toolTip_)
    {
    }

    PrimaryField primaryField = index;
    QStringList model;
};

struct ComboBoxGroup : ComboBox, Group
{
    ComboBoxGroup() { }
    ComboBoxGroup(
        const ctti::unnamed_type_id_t &type_, const QString &desc_, const u16 &group_, const QString &toolTip_)
        : ComboBox(type_, desc_, group_, toolTip_)
    {
    }
};
}

namespace config
{

// Unique item, not mergeable
struct Item : delegate::Widget
{
    enum ModbusColumns : int
    {
        SensorType = 0,
        BaudRate,
        Parity,
        StopBits,
        Period,
        Address,
        DataType,
        FuncCode,
        Register,
        Count
    };

    Item() { }
    Item(const ctti::unnamed_type_id_t type_) : Widget(type_) { }

    Item(const ctti::unnamed_type_id_t &type_, const delegate::ItemType &itype_, //
        const quint16 &parent_, const u16 &group_)
        : Widget(type_, group_)
        , itemType(itype_)
        , parent(parent_)
    {
    }

    delegate::ItemType itemType;
    quint16 parent;
};

using itemVariant = std::variant<  //
    delegate::Widget,              //
    delegate::ComboBox,            //
    delegate::DoubleSpinBoxGroup,  //
    delegate::DoubleSpinBoxWidget, //
    delegate::CheckBoxGroup,       //
    delegate::ComboBoxGroup,       //
    Item                           //
    >;

using widgetMap = QMap<u32, itemVariant>;
}
