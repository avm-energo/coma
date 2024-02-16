#ifndef DELEGATE_COMMON_H
#define DELEGATE_COMMON_H

#include <QMap>
#include <QString>
#include <ctti/type_id.hpp>
#include <variant>

namespace delegate
{
using WidgetGroup = int;

// TODO: так и должно быть?
enum class ItemType : int
{
    ModbusItem
};

struct Widget
{
    Widget(const ctti::unnamed_type_id_t type_) : type(type_)
    {
    }

    Widget(const ctti::unnamed_type_id_t type_, const WidgetGroup group_) : type(type_), group(group_)
    {
    }

    Widget(
        const ctti::unnamed_type_id_t type_, const QString &desc_, const WidgetGroup &group_, const QString &toolTip_)
        : type(type_), desc(desc_), group(group_), toolTip(toolTip_)
    {
    }

    ctti::unnamed_type_id_t type;
    QString desc;
    WidgetGroup group;
    QString toolTip;
};

struct DoubleSpinBoxWidget : Widget
{
    DoubleSpinBoxWidget(const ctti::unnamed_type_id_t &type_, const QString &desc_, //
        const WidgetGroup &group_, const QString &toolTip_)                         //
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
    DoubleSpinBoxGroup(
        const ctti::unnamed_type_id_t &type_, const QString &desc_, const WidgetGroup &group_, const QString &toolTip_)
        : DoubleSpinBoxWidget(type_, desc_, group_, toolTip_)
    {
    }
};

struct CheckBoxGroup : Widget, Group
{
    CheckBoxGroup(
        const ctti::unnamed_type_id_t &type_, const QString &desc_, const WidgetGroup &group_, const QString &toolTip_)
        : Widget(type_, desc_, group_, toolTip_)
    {
    }
};

struct QComboBox : Widget
{
    enum PrimaryField : int
    {
        index = 0,
        data = 1,
        bitfield = 2
    };

    QComboBox(
        const ctti::unnamed_type_id_t &type_, const QString &desc_, const WidgetGroup &group_, const QString &toolTip_)
        : Widget(type_, desc_, group_, toolTip_)
    {
    }

    PrimaryField primaryField = index;
    QStringList model;
};

struct QComboBoxGroup : QComboBox, Group
{
    QComboBoxGroup(
        const ctti::unnamed_type_id_t &type_, const QString &desc_, const WidgetGroup &group_, const QString &toolTip_)
        : QComboBox(type_, desc_, group_, toolTip_)
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
        Timeout,
        Address,
        FuncCode,
        DataType,
        Register
    };

    Item(const ctti::unnamed_type_id_t type_) : Widget(type_)
    {
    }

    Item(const ctti::unnamed_type_id_t &type_, const delegate::ItemType &itype_, //
        const quint16 &parent_, const delegate::WidgetGroup &group_)
        : Widget(type_, group_), itemType(itype_), parent(parent_)
    {
    }

    delegate::ItemType itemType;
    quint16 parent;
};

using itemVariant = std::variant<  //
    delegate::Widget,              //
    delegate::QComboBox,           //
    delegate::DoubleSpinBoxGroup,  //
    delegate::DoubleSpinBoxWidget, //
    delegate::CheckBoxGroup,       //
    delegate::QComboBoxGroup,      //
    Item                           //
    >;

using widgetMap = std::map<quint32, itemVariant>;
};

#endif // DELEGATE_COMMON_H
