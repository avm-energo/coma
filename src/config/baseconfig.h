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
enum class ItemType : int
{
    ModbusItem
};
Q_ENUM_NS(widgetType)
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
    QComboBox(const ctti::unnamed_type_id_t type_) : Widget(type_)
    {
    }
};
struct Item : Widget
{
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

// helper type for the visitor
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class Module;
class QStandardItem;
class WidgetFactory
{
    friend class Module;

    template <typename T> static QWidget *helper(const T &arg, QWidget *parent)
    {
        QWidget *widget = nullptr;
        switch (arg.type.hash())
        {
        case ctti::unnamed_type_id<IPCtrl>().hash():
        {
            widget = new QWidget(parent);
            QHBoxLayout *lyout = new QHBoxLayout;
            lyout->addWidget(new QLabel(arg.desc, parent));
            lyout->addWidget(new IPCtrl(parent));
            widget->setLayout(lyout);
            break;
        }
        case ctti::unnamed_type_id<QCheckBox>().hash():
        {
            widget = new QCheckBox(arg.desc, parent);
            break;
        }
        case ctti::unnamed_type_id<QLineEdit>().hash():
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

public:
    WidgetFactory()
    {
    }
    QWidget *createWidget(BciNumber key, QWidget *parent = nullptr);
    template <typename T> bool fillWidget(QWidget *parent, BciNumber key, const T &value)
    {
        bool status = false;
        auto search = widgetMap.find(key);
        if (search == widgetMap.end())
        {
            qWarning() << "Not found" << key;
            return status;
        }

        const auto var = search->second;
        std::visit(overloaded {
                       [&](const auto &arg) {
                           qDebug("DefaultWidget");
                           using namespace delegate;

                           switch (arg.type.hash())
                           {
                           case ctti::unnamed_type_id<IPCtrl>().hash():
                           {
                               auto widget = parent->findChild<IPCtrl *>(QString::number(key));
                               if (!widget)
                                   return;
                               if constexpr (std::is_container<T>())
                                   if constexpr (std::is_same<decltype(value), IPCtrl::value_type>::value)
                                       widget->setIP(value);
                               status = true;
                               break;
                           }
                           case ctti::unnamed_type_id<QCheckBox>().hash():
                           {
                               auto widget = parent->findChild<QCheckBox *>(QString::number(key));
                               if (!widget)
                                   return;
                               if constexpr (!std::is_container<T>())
                                   widget->setChecked(bool(value));
                               status = true;
                               break;
                           }
                           case ctti::unnamed_type_id<QLineEdit>().hash():
                           {
                               auto widget = parent->findChild<QLineEdit *>(QString::number(key));
                               if (!widget)
                                   return;
                               if constexpr (!std::is_container<T>())
                                   widget->setText(QString::number(value));
                               status = true;
                               break;
                           }

                           default:
                               break;
                               Q_ASSERT(false && "False type");
                           }
                       },
                       [&](const delegate::DoubleSpinBoxGroup &arg) {
                           qDebug("DoubleSpinBoxGroupWidget");
                           if constexpr (std::is_container<T>())
                               if constexpr (sizeof(T::value_type) != 1 && !std::is_container<typename T::value_type>())
                                   status = WDFunc::SetSPBGData(parent, QString::number(key), value);
                       },
                       [&](const delegate::DoubleSpinBoxWidget &arg) {
                           qDebug("DoubleSpinBoxWidget");
                           if constexpr (!std::is_container<T>())
                               status = WDFunc::SetSPBData(parent, QString::number(key), value);
                       },
                       [&](const delegate::CheckBoxGroup &arg) {
                           qDebug("CheckBoxGroupWidget");
                           if constexpr (std::is_same<T, quint32>::value || std::is_same<T, quint64>::value)
                               status = WDFunc::SetChBGData(parent, QString::number(key), value);
                       },
                       [&](const delegate::QComboBox &arg) {
                           qDebug("QComboBox");
                           if constexpr (!std::is_container<T>())
                               status = WDFunc::SetCBIndex(parent, QString::number(key), value);
                       },
                   },
            var);
        return status;
    }
    static QStandardItem *createItem(BciNumber key, QWidget *parent = nullptr);

private:
private:
    static QWidget *createModbusView(QWidget *parent);
    static widgetMap widgetMap;
};

template <> QWidget *WidgetFactory::helper(const delegate::Item &arg, QWidget *parent);

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
