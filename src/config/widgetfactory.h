#pragma once
#include "../gen/std_ext.h"
#include "../s2/s2datatypes.h"
#include "../widgets/delegate_common.h"
#include "../widgets/ipctrl.h"
#include "../widgets/wd_func.h"

#include <QStandardItemModel>
#include <bitset>

//#define DEBUG_FACTORY

class ConfigV;

class WidgetFactory
{
    friend class Module;

public:
    WidgetFactory(ConfigV *config);
    QWidget *createWidget(quint16 key, QWidget *parent = nullptr);
    template <typename T> bool fillWidget(const QWidget *parent, quint16 key, const T &value);
    bool fillBack(quint16 key, const QWidget *parent);

    static QString hashedName(ctti::unnamed_type_id_t type, quint16 key)
    {
        return QString::number(type.hash()) + QString::number(key);
    }
    static auto getWidgetMap()
    {
        return m_widgetMap;
    }
    static auto getCategoryMap()
    {
        return m_categoryMap;
    }
    static auto getCategory(delegate::WidgetGroup key)
    {
        return m_categoryMap.value(key);
    }

private:
    // Default template like a dummy function for sfinae
    // We will be here if specialisation doesn't exist for this T
    template <typename T>
    QList<QStandardItem *> createItem(
        [[maybe_unused]] quint16 key, [[maybe_unused]] const T &value, [[maybe_unused]] const QWidget *parent = nullptr)
    {
        return {};
    };

    template <typename T, std::enable_if_t<std::is_same<T, IPCtrl::container_type>::value, bool> = true>
    bool fillIpCtrl(const QWidget *parent, quint16 key, const T &value);
    bool fillCheckBox(const QWidget *parent, quint16 key, bool value);
    template <typename T, std::enable_if_t<!std_ext::is_container<T>::value, bool> = true>
    bool fillLineEdit(const QWidget *parent, quint16 key, const T &value);
    template <typename T, std::enable_if_t<std_ext::is_container<T>::value, bool> = true>
    bool fillLineEdit(const QWidget *parent, quint16 key, const T &value);
    template <typename T>
    bool fillTableView(const QWidget *parent, quint16 key, quint16 parentKey, //
        ctti::unnamed_type_id_t type, const T &value);

    template <typename T> bool fillBackItem(quint16 key, const QWidget *parent, quint16 parentKey);

    // helpers for fill back from widget
    bool fillBackModbus(quint16 key, const QWidget *parent, ctti::unnamed_type_id_t type, quint16 parentKey);
    bool fillBackIpCtrl(quint16 key, const QWidget *parent);
    bool fillBackCheckBox(quint16 key, const QWidget *parent);
    bool fillBackLineEdit(quint16 key, const QWidget *parent);
    bool fillBackSPBG(quint16 key, const QWidget *parent);
    bool fillBackSPB(quint16 key, const QWidget *parent);
    bool fillBackChBG(quint16 key, const QWidget *parent);
    bool fillBackComboBox(quint16 key, const QWidget *parent, delegate::QComboBox::PrimaryField field);
    bool fillBackComboBoxGroup(quint16 key, const QWidget *parent, int count);

    static config::widgetMap m_widgetMap;
    static categoryMap m_categoryMap;
    ConfigV *configV;
};

inline bool WidgetFactory::fillCheckBox(const QWidget *parent, quint16 key, bool value)
{
    auto widget = parent->findChild<QCheckBox *>(QString::number(key));
    if (!widget)
        return false;
    widget->setChecked(bool(value));
    return true;
}

const inline QString widgetName(int group, int item)
{
    return QString::number(group) + "-" + QString::number(item);
}

// Template specialisation

template <>
QList<QStandardItem *> WidgetFactory::createItem(quint16 key, const DataTypes::BYTE_8t &value, const QWidget *parent);

// Template definition

template <typename T, std::enable_if_t<std::is_same<T, IPCtrl::container_type>::value, bool>>
bool WidgetFactory::fillIpCtrl(const QWidget *parent, quint16 key, const T &value)
{
    auto widget = parent->findChild<IPCtrl *>(QString::number(key));
    if (!widget)
        return false;
    widget->setIP(value);
    return true;
}

template <typename T, std::enable_if_t<!std_ext::is_container<T>::value, bool>>
bool WidgetFactory::fillLineEdit(const QWidget *parent, quint16 key, const T &value)
{
    auto widget = parent->findChild<QLineEdit *>(QString::number(key));
    if (!widget)
        return false;
    widget->setText(QString::number(value));
    return true;
}

template <typename T, std::enable_if_t<std_ext::is_container<T>::value, bool>>
bool WidgetFactory::fillLineEdit(const QWidget *parent, quint16 key, const T &value)
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
    const QWidget *parent, quint16 key, quint16 parentKey, ctti::unnamed_type_id_t type, const T &value)
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

template <typename T> bool WidgetFactory::fillWidget(const QWidget *parent, quint16 key, const T &value)
{
    bool status = false;
    auto search = m_widgetMap.find(key);
    if (search == m_widgetMap.end())
    {
        if (key == 0)
        {
            qWarning() << "No S2 item for this widget";
            return status;
        }
        qWarning() << "Not found" << key;
        return status;
    }

    const auto var = search->second;
    std::visit(overloaded {
                   [&](const auto &arg) {
#ifdef DEBUG_FACTORY
                       qDebug() << "DefaultWidget" << key;
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
                       if constexpr (!std_ext::is_container<T>())
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
                       if constexpr (std_ext::is_container<T>())
                           if constexpr (sizeof(typename T::value_type) != 1
                               && !std_ext::is_container<typename T::value_type>())
                           {
#ifdef DEBUG_FACTORY
                               qDebug() << "DoubleSpinBoxGroupWidget" << key;
#endif
                               status = WDFunc::SetSPBGData(parent, QString::number(key), value);
                           }
                   },
                   [&]([[maybe_unused]] const delegate::DoubleSpinBoxWidget &arg) {
                       if constexpr (!std_ext::is_container<T>())
                       {
#ifdef DEBUG_FACTORY
                           qDebug() << "DoubleSpinBoxWidget" << key;
#endif
                           status = WDFunc::SetSPBData(parent, QString::number(key), value);
                       }
                   },
                   [&]([[maybe_unused]] const delegate::CheckBoxGroup &arg) {
                       if constexpr (std::is_unsigned_v<T>)
                       {
#ifdef DEBUG_FACTORY
                           qDebug() << "CheckBoxGroupWidget" << key;
#endif
                           status = WDFunc::SetChBGData(parent, QString::number(key), value);
                       }
                       else if constexpr (std_ext::is_container<T>())
                       {
                           typedef std::remove_reference_t<typename T::value_type> internalType;
                           if constexpr (std::is_unsigned_v<internalType>)
                           {
#ifdef DEBUG_FACTORY
                               qDebug() << "CheckBoxGroupWidget" << key;
#endif
                               status = WDFunc::SetChBGData(parent, QString::number(key), value);
                           }
                       }
                   },
                   [&](const delegate::QComboBox &arg) {
                       if constexpr (!std_ext::is_container<T>())
                       {
#ifdef DEBUG_FACTORY
                           qDebug() << "QComboBox" << key;
#endif
                           switch (arg.primaryField)
                           {
                           case delegate::QComboBox::data:
                           {
                               auto index = arg.model.indexOf(QString::number(value));
                               if (index != -1)
                                   status = WDFunc::SetCBIndex(parent, QString::number(key), index);
                               break;
                           }
                           default:
                           {
                               if constexpr (std::is_integral_v<T>)
                                   status = WDFunc::SetCBIndex(parent, QString::number(key), value);
                               break;
                           }
                           }
                       }
                   },
                   [&](const delegate::QComboBoxGroup &arg) {
                       if constexpr (!std_ext::is_container<T>())
                       {
#ifdef DEBUG_FACTORY
                           qDebug() << "QComboBoxGroup" << key;
#endif
                           std::bitset<sizeof(T) *CHAR_BIT> bitset = value;
                           int count = arg.count;
                           bool flag = false;
                           for (auto i = 0; i != count; ++i)
                           {
                               status = WDFunc::SetCBIndex(parent, widgetName(key, i), bitset.test(i));
                               Q_ASSERT(status && "Couldn't fill QComboBox");
                               if (!status && !flag)
                               {
                                   flag = true;
                               }
                               if (flag)
                               {
                                   status = false;
                               }
                           }
                       }
                       else if constexpr (std_ext::is_container<T>())
                       {
                           typedef std::remove_reference_t<typename T::value_type> internalType;
                           if constexpr (std::is_unsigned_v<internalType>)
                           {
#ifdef DEBUG_FACTORY
                               qDebug() << "QComboBoxGroup" << key;
#endif
                               auto count = std::min(std::size_t(arg.count), value.size());
                               bool flag = false;
                               for (auto i = 0; i != count; ++i)
                               {
                                   status = WDFunc::SetCBIndex(parent, widgetName(key, i), value.at(i));
                                   Q_ASSERT(status && "Couldn't fill QComboBox");
                                   if (!status && !flag)
                                   {
                                       flag = true;
                                   }
                                   if (flag)
                                   {
                                       status = false;
                                   }
                               }
                           }
                       }
                   },
                   [&](const config::Item &arg) {
#ifdef DEBUG_FACTORY
                       qDebug() << "Item" << key;
#endif
                       status = fillTableView(parent, key, arg.parent, arg.type, value);
                   },
               },
        var);
    return status;
}
