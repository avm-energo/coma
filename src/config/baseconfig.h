#pragma once
#include "../ctti/type_id.hpp"
#include "../gen/s2.h"
#include "../module/modules.h"
#include "../widgets/ipctrl.h"
#include "../widgets/wd_func.h"

#include <cstdint>
#include <type_traits>
constexpr int textColumn = 0;
constexpr int valueColumn = 1;
constexpr int extraColumn = 2;

namespace delegate
{
Q_NAMESPACE
constexpr auto QLabelId = ctti::unnamed_type_id<QLabel>();
constexpr auto QDoubleSpinBoxId = ctti::unnamed_type_id<QDoubleSpinBox>();
constexpr auto IpControlId = ctti::unnamed_type_id<IPCtrl>();
constexpr auto QCheckBoxId = ctti::unnamed_type_id<QCheckBox>();
constexpr auto QLineEditId = ctti::unnamed_type_id<QLineEdit>();
constexpr auto DoubleSpinBoxGroupId = ctti::unnamed_type_id<DoubleSpinBoxGroup>();
constexpr auto QComboBoxId = ctti::unnamed_type_id<QComboBox>();
// constexpr auto ModbusItemId = ctti::unnamed_type_id<AVM_KXX::ModbusItem>();
enum class widgetType : int
{
    dummyElement = 0,
    QLabel,
    QDoubleSpinBox,
    IpControl,
    QCheckBox,
    QLineEdit,
    // Group like a separator
    Group = 100,
    DoubleSpinBoxGroup,
    CheckBoxGroup,
    QComboBox,
    // Item like a separator
    // Item = 200,
    // ModbusItem

};
enum class itemType : int
{
    ModbusItem
};
Q_ENUM_NS(widgetType)
Q_ENUM_NS(itemType)
struct Widget
{
    widgetType type;
    QString desc;
};

struct DoubleSpinBoxWidget : Widget
{
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
};
struct CheckBoxGroup : Widget, Group
{
};
struct QComboBox : Widget, Group
{
};
struct Item
{
    itemType type;
    BciNumber parent;
};

using itemVariant = std::variant<Widget, QComboBox, DoubleSpinBoxGroup, DoubleSpinBoxWidget, CheckBoxGroup, Item>;
}
using widgetMap = std::map<BciNumber, delegate::itemVariant>;

// helper type for the visitor
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename T, typename = int> struct hasDescription : std::false_type
{
};

template <typename T> struct hasDescription<T, decltype((void)T::desc, 0)> : std::true_type
{
};

class Module;
class QStandardItem;
class WidgetFactory
{
    friend class Module;
    WidgetFactory()
    {
    }
    //  template <typename T, std::enable_if_t<std::is_same<std::string, decltype(declval<T>().desc)>>, bool> = true >
    // std::enable_if_t<true_type<T>::value, bool> = true
    // template <typename T, std::enable_if_t<std::is_same<QString, decltype(T::desc)>::value, bool> = true>
    template <typename T, typename S = decltype(T::type),
        std::enable_if_t<std::is_same<S, delegate::widgetType>::value, bool> = true>
    static QWidget *helper(const T &arg, QWidget *parent)
    {
        QWidget *widget = nullptr;
        switch (arg.type)
        {
        case delegate::widgetType::IpControl:
        {
            widget = new QWidget(parent);
            QHBoxLayout *lyout = new QHBoxLayout;
            lyout->addWidget(new QLabel(arg.desc, parent));
            lyout->addWidget(new IPCtrl(parent));
            widget->setLayout(lyout);
            break;
        }
        case delegate::widgetType::QCheckBox:
        {
            widget = new QCheckBox(arg.desc, parent);
            break;
        }
        case delegate::widgetType::QLineEdit:
        {
            widget = new QWidget(parent);
            QHBoxLayout *lyout = new QHBoxLayout;
            lyout->addWidget(new QLabel(arg.desc, parent));
            lyout->addWidget(new QLineEdit(parent));
            widget->setLayout(lyout);
            break;
        }

        default:
            break;
            Q_ASSERT(false && "False type");
        }
        return widget;
    }
    template <typename T, typename S = decltype(T::type),
        std::enable_if_t<std::is_same<S, delegate::itemType>::value, bool> = true>
    static QWidget *helper(const T &arg, QWidget *parent)
    {
        QWidget *widget = nullptr;
        switch (arg.type)
        {
        case delegate::itemType::ModbusItem:
        {
            auto type = static_cast<const std::underlying_type<delegate::itemType>::type>(arg.type);
            const QString widgetName(QString::number(type) + QString::number(arg.parent));
            widget = parent->findChild<QTableView *>(widgetName);
            if (!widget)
            {
                widget = createModbusView(parent);
                widget->setObjectName(widgetName);
            }
            return widget;
        }
        }
        return nullptr;
    }

public:
    static QWidget *createWidget(BciNumber key, QWidget *parent = nullptr);
    static QStandardItem *createItem(BciNumber key, QWidget *parent = nullptr);

private:
private:
    static QWidget *createModbusView(QWidget *parent);
    static widgetMap widgetMap;
};

class BaseConfig
{
public:
    BaseConfig();
    virtual ~BaseConfig() {};
    // Записать конфигурацию по умолчанию в структуру конфигурации
    virtual void SetDefConf() = 0;
    // заполнить значения полей вывода из структуры конфигурации
    virtual void Fill() = 0;
    // ввести информацию из полей вывода в конфигурацию
    virtual void FillBack() const = 0;
    // проверить конфигурацию на корректность, признаком наличия некорректностей является непустой список
    // реализовать или отказаться
    virtual void CheckConf() const {};

    inline QString nameByValue(quint32 number) const
    {
        return QString::number(number);
    }
    template <typename T> inline bool SetCBIndex(const QObject *parent, const unsigned number) const
    {
        return WDFunc::SetCBIndex(parent, nameByValue(number), S2::getRecord(number).value<T>());
    }
    template <typename T> inline bool SetSPBData(QObject *parent, const unsigned number)
    {
        return WDFunc::SetSPBData(parent, nameByValue(number), S2::getRecord(number).value<T>());
    }
    template <typename T> inline void SPBDataS2(const QObject *parent, const unsigned number) const
    {
        S2::setRecordValue({ number, WDFunc::SPBData<T>(parent, nameByValue(number)) });
    }
};
