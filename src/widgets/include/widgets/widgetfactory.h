#pragma once

#include <QStandardItemModel>
#include <bitset>
#include <device/current_device.h>
#include <gen/std_ext.h>
#include <s2/delegate_common.h>
#include <widgets/gasdensitywidget.h>
#include <widgets/ipctrl.h>
#include <widgets/wd_func.h>

class WidgetFactory
{
private:
    S2::Configuration &m_config;
    Device::ConfigStorage &m_storage;
    S2::ConfigStorage &m_s2storage;

public:
    WidgetFactory(S2::Configuration &workingConfig, Device::CurrentDevice *device);
    QWidget *createWidget(quint16 key, QWidget *parent = nullptr);
    template <typename T> bool fillWidget(const QWidget *parent, quint32 key, const T &value);
    bool fillBack(quint16 key, const QWidget *parent) const;
    static QString hashedName(ctti::unnamed_type_id_t type, quint16 key);
    static const QString widgetName(int group, int item);
    quint16 getRealCount(const quint16 key);

private:
    // Default template like a dummy function for sfinae
    // We will be here if specialisation doesn't exist for this T
    template <typename T>
    QList<QStandardItem *> createItem(
        [[maybe_unused]] quint16 key, [[maybe_unused]] const T &value, [[maybe_unused]] const QWidget *parent = nullptr)
    {
        return {};
    };

    template <typename T, std::enable_if_t<std::is_same<T, IPCtrl::ip_container>::value, bool> = true>
    bool fillIpCtrl(const QWidget *parent, quint16 key, const T &value);
    template <typename T, std::enable_if_t<!std_ext::is_container<T>::value, bool> = true>
    bool fillLineEdit(const QWidget *parent, quint16 key, const T &value);
    template <typename T, std::enable_if_t<std_ext::is_container<T>::value, bool> = true>
    bool fillLineEdit(const QWidget *parent, quint16 key, const T &value);
    template <typename T>
    bool fillTableView(const QWidget *parent, quint16 key, quint16 parentKey, //
        ctti::unnamed_type_id_t type, const T &value);
    bool fillCheckBox(const QWidget *parent, quint16 key, bool value);
    bool fillGasWidget(const QWidget *parent, quint16 key, const S2::CONF_DENS_3t &value);

    template <typename T> bool fillBackItem(quint16 key, const QWidget *parent, quint16 parentKey) const;

    // helpers for fill back from widget
    bool fillBackModbus(quint32 id, const QWidget *parent, ctti::unnamed_type_id_t type, quint16 parentKey) const;
    bool fillBackIpCtrl(quint32 id, const QWidget *parent) const;
    bool fillBackCheckBox(quint32 id, const QWidget *parent) const;
    bool fillBackLineEdit(quint32 id, const QWidget *parent) const;
    bool fillBackSPBG(quint32 id, const QWidget *parent) const;
    bool fillBackSPB(quint32 id, const QWidget *parent) const;
    bool fillBackChBG(quint32 id, const QWidget *parent) const;
    bool fillBackComboBox(quint32 id, const QWidget *parent, delegate::QComboBox::PrimaryField field) const;
    bool fillBackComboBoxGroup(quint32 id, const QWidget *parent, int count) const;
    bool fillBackGasWidget(quint32 id, const QWidget *parent) const;
};

// Template specialisation

template <>
QList<QStandardItem *> WidgetFactory::createItem(quint16 key, const S2::BYTE_8t &value, const QWidget *parent);

// Template definition

template <typename T, std::enable_if_t<std::is_same<T, IPCtrl::ip_container>::value, bool>>
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

template <typename T> bool WidgetFactory::fillWidget(const QWidget *parent, quint32 key, const T &value)
{
    bool status = false;
    // auto &widgetMap = S2::ConfigStorage::GetInstance().getWidgetMap();
    auto &widgetMap = m_s2storage.getWidgetMap();
    auto search = widgetMap.find(key);
    if (search == widgetMap.cend())
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
                       using namespace delegate;
                       if constexpr (std::is_same<T, IPCtrl::ip_container>::value)
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
                       if constexpr (std::is_same_v<T, S2::CONF_DENS_3t>)
                       {
                           if (arg.type == ctti::unnamed_type_id<GasDensityWidget>())
                           {
                               status = fillGasWidget(parent, key, value);
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
                       {
                           using container_type = typename T::value_type;
                           if constexpr (sizeof(container_type) != 1 &&    //
                               !std_ext::is_container<container_type>() && //
                               !std::is_same_v<container_type, S2::CONF_DENS>)
                           {
                               status = WDFunc::SetSPBGData(parent, QString::number(key), value);
                           }
                       }
                   },

                   [&]([[maybe_unused]] const delegate::DoubleSpinBoxWidget &arg) {
                       if constexpr (!std_ext::is_container<T>())
                       {
                           status = WDFunc::SetSPBData(parent, QString::number(key), value);
                       }
                   },
                   [&]([[maybe_unused]] const delegate::CheckBoxGroup &arg) {
                       if constexpr (std::is_unsigned_v<T>)
                       {
                           status = WDFunc::SetChBGData(parent, QString::number(key), value);
                       }
                       else if constexpr (std_ext::is_container<T>())
                       {
                           typedef std::remove_reference_t<typename T::value_type> internalType;
                           if constexpr (std::is_unsigned_v<internalType>)
                           {
                               status = WDFunc::SetChBGData(parent, QString::number(key), value);
                           }
                       }
                   },
                   [&](const delegate::QComboBox &arg) {
                       if constexpr (!std_ext::is_container<T>())
                       {
                           switch (arg.primaryField)
                           {
                           case delegate::QComboBox::data:
                           {
                               QString strValue;
                               if constexpr (std::is_floating_point_v<T>)
                               {
                                   strValue = QString::number(value, 'f', 2);
                                   strValue.replace('.', ',');
                               }
                               else
                                   strValue = QString::number(value);
                               auto index = arg.model.indexOf(strValue);
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
                           std::bitset<sizeof(T) *CHAR_BIT> bitset = value;
                           auto count = getRealCount(key);
                           bool flag = false;
                           for (auto i = 0; i != count; ++i)
                           {
                               status = WDFunc::SetCBIndex(parent, widgetName(key, i), bitset.test(i));
                               // Q_ASSERT(status && "Couldn't fill QComboBox");
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
                   [&](const config::Item &arg) { status = fillTableView(parent, key, arg.parent, arg.type, value); },
               },
        var);
    return status;
}
