#pragma once
#include "../ctti/type_id.hpp"
#include "../gen/s2.h"
#include "../module/modules.h"
#include "../widgets/ipctrl.h"
#include "../widgets/wd_func.h"

#include <QStandardItemModel>

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

    template <typename T> QWidget *helper(const T &arg, QWidget *parent, BciNumber key)
    {
        QWidget *widget = nullptr;
        switch (arg.type.hash())
        {
        case ctti::unnamed_type_id<IPCtrl>().hash():
        {
            widget = new QWidget(parent);
            QHBoxLayout *lyout = new QHBoxLayout;
            lyout->addWidget(new QLabel(arg.desc, parent));
            auto control = new IPCtrl(parent);
            control->setObjectName(QString::number(key));
            lyout->addWidget(control);
            widget->setLayout(lyout);
            break;
        }
        case ctti::unnamed_type_id<QCheckBox>().hash():
        {
            widget = new QCheckBox(arg.desc, parent);
            widget->setObjectName(QString::number(key));
            break;
        }
        case ctti::unnamed_type_id<QLineEdit>().hash():
        {
            widget = new QWidget(parent);
            QHBoxLayout *lyout = new QHBoxLayout;
            lyout->addWidget(new QLabel(arg.desc, parent));
            auto lineEdit = new QLineEdit(parent);
            lineEdit->setObjectName(QString::number(key));
            lyout->addWidget(lineEdit);
            widget->setLayout(lyout);
            break;
        }

        default:
            break;
            Q_ASSERT(false && "False type");
        }

        return widget;
    }
    template <typename T> bool fillIpCtrl(const QWidget *parent, BciNumber key, const T &value)
    {
        if constexpr (std::is_same<T, IPCtrl::container_type>::value)
        {
            auto widget = parent->findChild<IPCtrl *>(QString::number(key));
            if (!widget)
                return false;
            widget->setIP(value);
            return true;
        }
        return false;
    }
    template <typename T> bool fillCheckBox(const QWidget *parent, BciNumber key, const T &value)
    {
        if constexpr (!std::is_container<T>())
        {
            auto widget = parent->findChild<QCheckBox *>(QString::number(key));
            if (!widget)
                return false;
            widget->setChecked(bool(value));
            return true;
        }
        return false;
    }
    template <typename T> bool fillLineEdit(const QWidget *parent, BciNumber key, const T &value)
    {
        if constexpr (!std::is_container<T>())
        {
            auto widget = parent->findChild<QLineEdit *>(QString::number(key));
            if (!widget)
                return false;
            widget->setText(QString::number(value));
            return true;
        }
        if constexpr (std::is_container<T>())
            if constexpr (std::is_integral<typename T::value_type>::value)
            {
                auto widget = parent->findChild<QLineEdit *>(QString::number(key));
                if (!widget)
                    return false;
                widget->setText(QString::number(value.front()));
                return true;
            }
        return false;
    }
    template <typename T>
    bool fillTableView(
        const QWidget *parent, BciNumber key, BciNumber parentKey, ctti::unnamed_type_id_t type, const T &value)
    {
        const QString widgetName(QString::number(type.hash()) + QString::number(parentKey));
        auto tableView = parent->findChild<QTableView *>(widgetName);
        if (!tableView)
            return false;
        auto model = qobject_cast<QStandardItemModel *>(tableView->model());
        if (!model)
            return false;
        // Mountain bike or simple костыль
        // Excepts parent item key is N, when children start from N + 1
        int targetRow = key - parentKey - 1;
        auto row = model->takeRow(targetRow);
        if (!row.isEmpty())
            qDeleteAll(row);
        row = createItem(key, value, parent);
        model->insertRow(targetRow, row);
        return true;
    }

public:
    WidgetFactory()
    {
    }
    QWidget *createWidget(BciNumber key, QWidget *parent = nullptr);

    template <typename T> bool fillWidget(const QWidget *parent, BciNumber key, const T &value)
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
                               status = fillIpCtrl(parent, key, value);
                               break;
                           }
                           case ctti::unnamed_type_id<QCheckBox>().hash():
                           {
                               status = fillCheckBox(parent, key, value);
                               break;
                           }
                           case ctti::unnamed_type_id<QLineEdit>().hash():
                           {
                               status = fillLineEdit(parent, key, value);
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
                               {
                                   status = WDFunc::SetSPBGData(parent, QString::number(key), value);
                               }
                       },
                       [&](const delegate::DoubleSpinBoxWidget &arg) {
                           qDebug("DoubleSpinBoxWidget");
                           if constexpr (!std::is_container<T>())
                           {
                               status = WDFunc::SetSPBData(parent, QString::number(key), value);
                           }
                       },
                       [&](const delegate::CheckBoxGroup &arg) {
                           qDebug("CheckBoxGroupWidget");
                           if constexpr (std::is_same<std::remove_const_t<T>, quint32>::value
                               || std::is_same<std::remove_const_t<T>, quint64>::value)
                           {
                               status = WDFunc::SetChBGData(parent, QString::number(key), value);
                           }
                       },
                       [&](const delegate::QComboBox &arg) {
                           qDebug("QComboBox");
                           if constexpr (!std::is_container<T>())
                           {
                               status = WDFunc::SetCBIndex(parent, QString::number(key), value);
                           }
                       },
                       [&](const delegate::Item &arg) {
                           qDebug("Item");
                           status = fillTableView(parent, key, arg.parent, arg.type, value);
                       },
                   },
            var);
        return status;
    }
    bool fillBack(BciNumber key, const QWidget *parent);

private:
    template <typename T>
    QList<QStandardItem *> createItem(BciNumber key, const T &value, const QWidget *parent = nullptr)
    {
        return {};
    };
    QWidget *createModbusView(QWidget *parent);
    static widgetMap widgetMap;
};
template <>
QList<QStandardItem *> WidgetFactory::createItem(BciNumber key, const DataTypes::BYTE_8t &value, const QWidget *parent);
template <> QWidget *WidgetFactory::helper(const delegate::Item &arg, QWidget *parent, BciNumber key);