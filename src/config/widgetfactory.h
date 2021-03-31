#pragma once
#include "../gen/datatypes.h"
#include "../widgets/ipctrl.h"
#include "../widgets/wd_func.h"
#include "delegate.h"

#include <QStandardItemModel>

//#define DEBUG_FACTORY

// helper type for the visitor
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class WidgetFactory
{
    friend class Module;

public:
    WidgetFactory();
    QWidget *createWidget(BciNumber key, QWidget *parent = nullptr);
    template <typename T> bool fillWidget(const QWidget *parent, BciNumber key, const T &value);
    bool fillBack(BciNumber key, const QWidget *parent);

    static QString hashedName(ctti::unnamed_type_id_t type, BciNumber key)
    {
        return QString::number(type.hash()) + QString::number(key);
    }
    static auto getWidgetMap()
    {
        return widgetMap;
    }

private:
    // Default template like a dummy function for sfinae
    // We will be here if specialisation doesn't exist for this T
    template <typename T>
    QList<QStandardItem *> createItem([[maybe_unused]] BciNumber key, [[maybe_unused]] const T &value,
        [[maybe_unused]] const QWidget *parent = nullptr)
    {
        return {};
    };

    template <typename T, std::enable_if_t<std::is_same<T, IPCtrl::container_type>::value, bool> = true>
    bool fillIpCtrl(const QWidget *parent, BciNumber key, const T &value);
    template <typename T, std::enable_if_t<!std::is_container<T>::value, bool> = true>
    bool fillCheckBox(const QWidget *parent, BciNumber key, const T &value);
    template <typename T, std::enable_if_t<!std::is_container<T>::value, bool> = true>
    bool fillLineEdit(const QWidget *parent, BciNumber key, const T &value);
    template <typename T, std::enable_if_t<std::is_container<T>::value, bool> = true>
    bool fillLineEdit(const QWidget *parent, BciNumber key, const T &value);
    template <typename T>
    bool fillTableView(const QWidget *parent, BciNumber key, BciNumber parentKey, //
        ctti::unnamed_type_id_t type, const T &value);

    static widgetMap widgetMap;
};

// Template specialisation

template <>
QList<QStandardItem *> WidgetFactory::createItem(BciNumber key, const DataTypes::BYTE_8t &value, const QWidget *parent);

// Template definition

template <typename T, std::enable_if_t<std::is_same<T, IPCtrl::container_type>::value, bool>>
bool WidgetFactory::fillIpCtrl(const QWidget *parent, BciNumber key, const T &value)
{
    auto widget = parent->findChild<IPCtrl *>(QString::number(key));
    if (!widget)
        return false;
    widget->setIP(value);
    return true;
}

template <typename T, std::enable_if_t<!std::is_container<T>::value, bool>>
bool WidgetFactory::fillCheckBox(const QWidget *parent, BciNumber key, const T &value)
{
    auto widget = parent->findChild<QCheckBox *>(QString::number(key));
    if (!widget)
        return false;
    widget->setChecked(bool(value));
    return true;
}

template <typename T, std::enable_if_t<!std::is_container<T>::value, bool>>
bool WidgetFactory::fillLineEdit(const QWidget *parent, BciNumber key, const T &value)
{
    auto widget = parent->findChild<QLineEdit *>(QString::number(key));
    if (!widget)
        return false;
    widget->setText(QString::number(value));
    return true;
}

template <typename T, std::enable_if_t<std::is_container<T>::value, bool>>
bool WidgetFactory::fillLineEdit(const QWidget *parent, BciNumber key, const T &value)
{
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
bool WidgetFactory::fillTableView(
    const QWidget *parent, BciNumber key, BciNumber parentKey, ctti::unnamed_type_id_t type, const T &value)
{
    auto tableView = parent->findChild<QTableView *>(hashedName(type, parentKey));
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

template <typename T> bool WidgetFactory::fillWidget(const QWidget *parent, BciNumber key, const T &value)
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
#ifdef DEBUG_FACTORY
                       qDebug("DefaultWidget");
#endif
                       using namespace delegate;
                       if constexpr (std::is_same<T, IPCtrl::container_type>::value)
                       {
                           if (arg.type == ctti::unnamed_type_id<IPCtrl>())
                           {
                               status = fillIpCtrl(parent, key, value);
                               return;
                           }
                       }
                       if constexpr (!std::is_container<T>())
                       {
                           if (arg.type == ctti::unnamed_type_id<QCheckBox>())
                           {
                               status = fillCheckBox(parent, key, value);
                               return;
                           }
                       }
                       if (arg.type == ctti::unnamed_type_id<QLineEdit>())
                       {
                           status = fillLineEdit(parent, key, value);
                           return;
                       }
                   },
                   [&]([[maybe_unused]] const delegate::DoubleSpinBoxGroup &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("DoubleSpinBoxGroupWidget");
#endif
                       if constexpr (std::is_container<T>())
                           if constexpr (sizeof(T::value_type) != 1 && !std::is_container<typename T::value_type>())
                           {
                               status = WDFunc::SetSPBGData(parent, QString::number(key), value);
                           }
                   },
                   [&]([[maybe_unused]] const delegate::DoubleSpinBoxWidget &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("DoubleSpinBoxWidget");
#endif
                       if constexpr (!std::is_container<T>())
                       {
                           status = WDFunc::SetSPBData(parent, QString::number(key), value);
                       }
                   },
                   [&]([[maybe_unused]] const delegate::CheckBoxGroup &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("CheckBoxGroupWidget");
#endif
                       if constexpr (std::is_unsigned_v<T>)
                       {
                           status = WDFunc::SetChBGData(parent, QString::number(key), value);
                       }
                   },
                   [&](const delegate::QComboBox &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("QComboBox");
#endif
                       if constexpr (!std::is_container<T>())
                       {
                           switch (arg.primaryField)
                           {
                           case delegate::QComboBox::data:
                           {
                               auto index = arg.items.indexOf(QString::number(value));
                               if (index != -1)
                                   status = WDFunc::SetCBIndex(parent, QString::number(key), index);
                               break;
                           }
                           default:
                           {
                               status = WDFunc::SetCBIndex(parent, QString::number(key), value);
                               break;
                           }
                           }
                       }
                   },
                   [&](const delegate::Item &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("Item");
#endif
                       status = fillTableView(parent, key, arg.parent, arg.type, value);
                   },
               },
        var);
    return status;
}
