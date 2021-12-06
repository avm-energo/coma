#include "widgetfactory.h"

#include "../gen/configv.h"
#include "../gen/module_kxx.h"
#include "../models/comboboxdelegate.h"
#include "../widgets/checkboxgroup.h"
#include "../widgets/flowlayout.h"
#include "../widgets/ipctrl.h"

#include <QHeaderView>
#include <QStandardItem>
config::widgetMap WidgetFactory::m_widgetMap;
categoryMap WidgetFactory::m_categoryMap;
// forward declarations
// helpers for create widget
static QWidget *createModbusView(QWidget *parent);

template <typename T> QWidget *helper(const T &arg, QWidget *parent, BciNumber key)
{
    QWidget *widget = new QWidget(parent);
    QHBoxLayout *lyout = new QHBoxLayout;
    auto label = new QLabel(arg.desc, parent);
    label->setToolTip(arg.toolTip);
    label->setToolTipDuration(60000);
    lyout->addWidget(label);
    widget->setLayout(lyout);
    switch (arg.type.hash())
    {
    case ctti::unnamed_type_id<IPCtrl>().hash():
    {
        auto control = new IPCtrl(parent);
        control->setObjectName(QString::number(key));
        lyout->addWidget(control);

        break;
    }
    case ctti::unnamed_type_id<QCheckBox>().hash():
    {
        auto checkbox = new QCheckBox(parent);
        checkbox->setObjectName(QString::number(key));
        lyout->addWidget(checkbox);

        break;
    }
    case ctti::unnamed_type_id<QLineEdit>().hash():
    {
        auto lineEdit = new QLineEdit(parent);
        auto sizePolicy = lineEdit->sizePolicy();
        sizePolicy.setHorizontalPolicy(QSizePolicy::Preferred);
        lineEdit->setSizePolicy(sizePolicy);
        lineEdit->setObjectName(QString::number(key));
        lyout->addWidget(lineEdit);

        break;
    }

    default:
        Q_ASSERT(false && "False type");
        widget->deleteLater();
        widget = nullptr;
        break;
    }

    return widget;
}

template <> QWidget *helper(const config::Item &arg, QWidget *parent, [[maybe_unused]] BciNumber key)
{
    QWidget *widget = nullptr;
    switch (arg.itemType)
    {
    case delegate::ItemType::ModbusItem:
    {
        widget = parent->findChild<QTableView *>(WidgetFactory::hashedName(arg.type, arg.parent));
        if (!widget)
        {
            widget = createModbusView(parent);
            widget->setObjectName(WidgetFactory::hashedName(arg.type, arg.parent));
        }
        return widget;
    }
    }
    return nullptr;
}

template <typename T> bool WidgetFactory::fillBackItem(BciNumber key, const QWidget *parent, BciNumber parentKey)
{
    switch (parentKey)
    {
    case BciNumber::MBMaster:
    {
        return fillBackModbus(key, parent, ctti::unnamed_type_id<QTableView>(), parentKey);
        break;
    }
    default:
        Q_ASSERT(false && "Unsupported type");
        return false;
    }
    // return true;
};

WidgetFactory::WidgetFactory(ConfigV *config) : configV(config)
{
}

QWidget *WidgetFactory::createWidget(BciNumber key, QWidget *parent)
{
    QWidget *widget = nullptr;
    auto search = m_widgetMap.find(key);
    if (search == m_widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return widget;
    }

    const auto var = search->second;
    std::visit(
        overloaded {
            [&](const delegate::DoubleSpinBoxGroup &arg) {
#ifdef DEBUG_FACTORY
                qDebug() << "DoubleSpinBoxGroupWidget" << key;
#endif
                widget = new QWidget(parent);
                QHBoxLayout *lyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                lyout->addWidget(label);

                QWidget *spbGroup;
                if (!arg.items.isEmpty())
                {
                    assert(arg.items.count() == arg.count);
                    spbGroup = WDFunc::NewSPBG(parent, QString::number(key), arg.items, arg.min, arg.max, arg.decimals);
                }
                else
                    spbGroup = WDFunc::NewSPBG(parent, QString::number(key), arg.count, arg.min, arg.max, arg.decimals);

                lyout->addWidget(spbGroup);
                widget->setLayout(lyout);
            },
            [&](const delegate::DoubleSpinBoxWidget &arg) {
#ifdef DEBUG_FACTORY
                qDebug() << "DoubleSpinBoxWidget" << key;
#endif
                widget = new QWidget(parent);
                QHBoxLayout *lyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                lyout->addWidget(label);
                lyout->addWidget(WDFunc::NewSPB2(parent, QString::number(key), arg.min, arg.max, arg.decimals));
                widget->setLayout(lyout);
            },
            [&](const delegate::CheckBoxGroup &arg) {
#ifdef DEBUG_FACTORY
                qDebug() << "CheckBoxGroupWidget" << key;
#endif

                widget = new QWidget(parent);
                QHBoxLayout *lyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                lyout->addWidget(label);

                auto group = new CheckBoxGroup(arg.items, arg.count, parent);
                group->setObjectName(QString::number(key));
                lyout->addWidget(group);
                widget->setLayout(lyout);
            },
            [&](const delegate::QComboBox &arg) {
#ifdef DEBUG_FACTORY
                qDebug() << "QComboBox" << key;
#endif

                widget = new QWidget(parent);
                QHBoxLayout *lyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                lyout->addWidget(label);
                lyout->addWidget(WDFunc::NewCB2(parent, QString::number(key), arg.model));
                widget->setLayout(lyout);
            },
            [&](const delegate::QComboBoxGroup &arg) {
#ifdef DEBUG_FACTORY
                qDebug() << "QComboBoxGroup" << key;
#endif

                widget = new QWidget(parent);

                QHBoxLayout *mainLyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                mainLyout->addWidget(label);

                int count = arg.count;

                FlowLayout *flowLayout = new FlowLayout;
                for (auto i = 0; i != count; ++i)
                {
                    QWidget *widget = new QWidget;
                    QHBoxLayout *layout = new QHBoxLayout;
                    layout->addWidget(new QLabel(QString::number(i + 1), parent));
                    layout->addWidget(WDFunc::NewCB2(parent, widgetName(key, i), arg.model));
                    widget->setLayout(layout);
                    flowLayout->addWidget(widget);
                }
                mainLyout->addLayout(flowLayout);
                widget->setLayout(mainLyout);
            },
            [&](const auto &arg) {
#ifdef DEBUG_FACTORY
                qDebug() << "DefaultWidget" << key;
#endif
                using namespace delegate;
                widget = helper(arg, parent, key);
            },
        },
        var);
    return widget;
}

bool WidgetFactory::fillBack(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto search = m_widgetMap.find(key);
    if (search == m_widgetMap.end())
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

                       switch (arg.type.hash())
                       {
                       case ctti::unnamed_type_id<IPCtrl>().hash():
                       {
                           status = fillBackIpCtrl(key, parent);
                           break;
                       }
                       case ctti::unnamed_type_id<QCheckBox>().hash():
                       {
                           status = fillBackCheckBox(key, parent);
                           break;
                       }
                       case ctti::unnamed_type_id<QLineEdit>().hash():
                       {
                           status = fillBackLineEdit(key, parent);
                           break;
                       }

                       default:
                           Q_ASSERT(false && "False type");
                           break;
                       }
                   },
                   [&]([[maybe_unused]] const delegate::DoubleSpinBoxGroup &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("DoubleSpinBoxGroupWidget");
#endif
                       status = fillBackSPBG(key, parent);
                   },
                   [&]([[maybe_unused]] const delegate::DoubleSpinBoxWidget &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("DoubleSpinBoxWidget");
#endif
                       status = fillBackSPB(key, parent);
                   },
                   [&]([[maybe_unused]] const delegate::CheckBoxGroup &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("CheckBoxGroupWidget");
#endif
                       status = fillBackChBG(key, parent);
                   },
                   [&](const delegate::QComboBox &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("QComboBox");
#endif
                       status = fillBackComboBox(key, parent, arg.primaryField);
                   },
                   [&](const delegate::QComboBoxGroup &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("QComboBoxGroup");
#endif
                       status = fillBackComboBoxGroup(key, parent, arg.count);
                   },
                   [&](const config::Item &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("Item");
#endif
                       auto record = configV->getRecord(key);
                       std::visit(
                           [&](auto &&type) {
                               typedef std::remove_reference_t<decltype(type)> internalType;
                               status = fillBackItem<internalType>(key, parent, arg.parent);
                           },
                           record.getData());
                   },
               },
        var);
    return status;
}

template <>
QList<QStandardItem *> WidgetFactory::createItem(
    BciNumber key, const DataTypes::BYTE_8t &value, [[maybe_unused]] const QWidget *parent)
{
    QList<QStandardItem *> items {};
    auto search = m_widgetMap.find(key);
    if (search == m_widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return items;
    }

    const auto var = search->second;
    std::visit(overloaded {
                   [&]([[maybe_unused]] const auto &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("DefaultWidget");
#endif
                       using namespace delegate;
                   },

                   [&](const config::Item &arg) {
#ifdef DEBUG_FACTORY
                       qDebug("Item");
#endif
                       switch (arg.itemType)
                       {
                       case delegate::ItemType::ModbusItem:
                       {
                           const auto *master = reinterpret_cast<const Bci::ModbusItem *>(&value);
                           items = {
                               (new QStandardItem(QString::number(master->typedat))),        //
                               (new QStandardItem(QString::number(master->parport.baud))),   //
                               (new QStandardItem(QString::number(master->parport.parity))), //
                               (new QStandardItem(QString::number(master->parport.stop))),   //
                               (new QStandardItem(QString::number(master->per))),            //
                               (new QStandardItem(QString::number(master->adr))),            //
                               (new QStandardItem(QString::number(master->type.reg))),       //
                               (new QStandardItem(QString::number(master->type.dat))),       //
                               (new QStandardItem(QString::number(master->reg)))             //
                           };
                       }
                       }
                   },
               },
        var);
    return items;
}

static QWidget *createModbusView(QWidget *parent)
{
    QTableView *tableView = new QTableView(parent);

    tableView->setSelectionMode(QAbstractItemView::NoSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableView->setShowGrid(false);
    tableView->setStyleSheet("QTableView {background-color: transparent;}");
    tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    ComboBoxDelegate *comboBoxdelegate = new ComboBoxDelegate({ "нет", "тип 1", "тип 2", "тип 3" }, tableView);
    tableView->setItemDelegateForColumn(0, comboBoxdelegate);
    const QStringList baudList { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

    comboBoxdelegate = new ComboBoxDelegate(baudList, tableView);
    tableView->setItemDelegateForColumn(1, comboBoxdelegate);

    comboBoxdelegate = new ComboBoxDelegate({ "нет", "even", "odd" }, tableView);
    tableView->setItemDelegateForColumn(2, comboBoxdelegate);

    comboBoxdelegate = new ComboBoxDelegate({ "1", "2" }, tableView);
    tableView->setItemDelegateForColumn(3, comboBoxdelegate);

    SpinBoxDelegate *spinBoxDelegate
        = new SpinBoxDelegate(0, std::numeric_limits<decltype(Bci::ModbusItem::per)>::max(), tableView);
    tableView->setItemDelegateForColumn(4, spinBoxDelegate);

    spinBoxDelegate = new SpinBoxDelegate(0, std::numeric_limits<decltype(Bci::ModbusItem::adr)>::max(), tableView);
    tableView->setItemDelegateForColumn(5, spinBoxDelegate);

    const QStringList funcs { "Coils", "Status", "Holding", "Input" };
    comboBoxdelegate = new ComboBoxDelegate(funcs, tableView);
    // Modbus функции начинаются с 1
    comboBoxdelegate->setOffset(1);
    tableView->setItemDelegateForColumn(6, comboBoxdelegate);

    const QStringList types { "Uint16", "Int16", "Bool", "Uint32", "Float" };
    comboBoxdelegate = new ComboBoxDelegate(types, tableView);
    tableView->setItemDelegateForColumn(7, comboBoxdelegate);

    spinBoxDelegate = new SpinBoxDelegate(0, std::numeric_limits<decltype(Bci::ModbusItem::reg)>::max(), tableView);
    tableView->setItemDelegateForColumn(8, spinBoxDelegate);

    QStandardItemModel *model = new QStandardItemModel(tableView);
    const QStringList header { "датчик", "скорость", "чётность", "стопБиты", "период опроса", "абонент", "функция",
        "данные", "регистр" };
    model->setHorizontalHeaderLabels(header);
    tableView->setModel(model);

    for (int column = 0; column < model->columnCount(); column++)
    {
        // NOTE Ужасный костыль
        int width = tableView->horizontalHeader()->fontMetrics().horizontalAdvance(header.at(column)) * 1.5;
        tableView->setColumnWidth(column, width);
    }

    return tableView;
}
bool WidgetFactory::fillBackModbus(
    BciNumber key, const QWidget *parent, ctti::unnamed_type_id_t type, BciNumber parentKey)
{
    auto tableView = parent->findChild<QTableView *>(WidgetFactory::hashedName(type, parentKey));

    if (tableView == nullptr)
    {
        qDebug("Пустой tv");
        return false;
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tableView->model());
    // -1 hardcoded as diff between parent element and first child element
    int row = key - parentKey - 1;

    Bci::ModbusItem master;

    for (int c = 0; c < model->columnCount(); ++c)
    {

        QModelIndex index = model->index(row, c);
        QVariant value = model->data(index);

        bool status = false;
        auto data = value.toUInt(&status);
        switch (c)
        {
        case config::Item::ModbusColumns::SensorType:
        {
            master.typedat = Bci::SensorType(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::BaudRate:
        {
            master.parport.baud = CommandsMBS::BaudRate(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::Parity:
        {
            master.parport.parity = CommandsMBS::Parity(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::StopBits:
        {
            master.parport.stop = CommandsMBS::StopBits(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::Timeout:
        {
            master.per = status ? data : 0;
            break;
        }
        case config::Item::ModbusColumns::Address:
        {
            master.adr = status ? data : 0;
            break;
        }
        case config::Item::ModbusColumns::FuncCode:
        {
            master.type.reg = CommandsMBS::Commands(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::DataType:
        {
            master.type.dat = CommandsMBS::TypeId(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::Register:
        {
            master.reg = (status ? data : 0);
            break;
        }
        default:
            break;
        }

        DataTypes::BYTE_8t masterBuffer = *reinterpret_cast<DataTypes::BYTE_8t *>(&master);
        configV->setRecordValue({ key, masterBuffer });
    }
    return true;
}

bool WidgetFactory::fillBackIpCtrl(BciNumber key, const QWidget *parent)
{
    auto widget = parent->findChild<IPCtrl *>(QString::number(key));
    if (!widget)
        return false;
    configV->setRecordValue({ key, widget->getIP() });

    return true;
}

bool WidgetFactory::fillBackCheckBox(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto widget = parent->findChild<QCheckBox *>(QString::number(key));
    if (!widget)
        return status;
    bool state = widget->isChecked();
    auto record = configV->getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (std::is_integral_v<internalType>)
            {
                internalType value = state;
                record.setData(value);
                status = true;
            }
        },
        record.getData());
    configV->setRecordValue(record);
    return status;
}

bool WidgetFactory::fillBackLineEdit(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto widget = parent->findChild<QLineEdit *>(QString::number(key));
    if (!widget)
        return status;
    const QString text = widget->text();
    auto record = configV->getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (!std_ext::is_container<internalType>())
            {
                const auto value = QVariant(text).value<internalType>();
                record.setData(value);
                status = true;
            }
            if constexpr (std_ext::is_container<internalType>())
                if constexpr (!std_ext::is_container<typename internalType::value_type>())
                {
                    const auto value = QVariant(text).value<typename internalType::value_type>();
                    arg.at(0) = value;
                    record.setData(arg);
                    status = true;
                }
        },
        record.getData());
    configV->setRecordValue(record);
    return status;
}

bool WidgetFactory::fillBackSPBG(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto record = configV->getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (std_ext::is_container<internalType>())
                if constexpr (sizeof(typename internalType::value_type) != 1
                    && !std_ext::is_container<typename internalType::value_type>())
                {
                    internalType buffer {};
                    status = WDFunc::SPBGData(parent, QString::number(key), buffer);
                    if (status)
                        configV->setRecordValue({ key, buffer });
                }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBackSPB(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto record = configV->getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (!std_ext::is_container<internalType>())
            {
                auto buffer = WDFunc::SPBData<internalType>(parent, QString::number(key));
                configV->setRecordValue({ key, buffer });
                status = true;
            }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBackChBG(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto record = configV->getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (!std_ext::is_container<internalType>())
            {
                if constexpr (std::is_unsigned_v<internalType>)
                {
                    internalType buffer = 0;
                    status = WDFunc::ChBGData(parent, QString::number(key), buffer);
                    if (status)
                        configV->setRecordValue({ key, buffer });
                }
            }

            else if constexpr (std_ext::is_container<internalType>())
            {
                typedef internalType Container;
                typedef std::remove_reference_t<typename internalType::value_type> internalType;
                if constexpr (std::is_integral<internalType>::value)
                {
                    Container buffer;
                    status = WDFunc::ChBGData(parent, QString::number(key), buffer);
                    if (status)
                        configV->setRecordValue({ key, buffer });
                }
            }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBackComboBox(BciNumber key, const QWidget *parent, delegate::QComboBox::PrimaryField field)
{
    bool status = false;
    auto record = configV->getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (std::is_arithmetic_v<internalType>)
            {
                switch (field)
                {
                case delegate::QComboBox::data:
                {
                    auto buffer = WDFunc::CBData<internalType>(parent, QString::number(key));
                    configV->setRecordValue({ key, buffer });
                    break;
                }
                case delegate::QComboBox::bitfield:
                {
                    // NOTE Nothing here
                }
                default:
                {
                    if constexpr (std::is_integral_v<internalType>)
                    {
                        int status_code = WDFunc::CBIndex(parent, QString::number(key));
                        if (status_code == -1)
                            return;
                        configV->setRecordValue({ key, static_cast<internalType>(status_code) });
                    }
                    break;
                }
                }
                status = true;
            }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBackComboBoxGroup(BciNumber key, const QWidget *parent, int count)
{
    bool status = false;
    auto record = configV->getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (std::is_unsigned_v<internalType>)
            {
                std::bitset<sizeof(internalType) *CHAR_BIT> bitset = 0;
                assert(size_t(count) <= bitset.size());
                status = true;
                for (int i = 0; i != count; ++i)
                {
                    int status_code = WDFunc::CBIndex(parent, widgetName(key, i));

                    if (status_code == -1)
                    {
                        status = false;
                        break;
                    }

                    bitset.set(i, bool(status_code));
                }
                configV->setRecordValue({ key, static_cast<internalType>(bitset.to_ullong()) });
            }
            else if constexpr (std_ext::is_container<internalType>())
            {
                typedef internalType Container;
                typedef std::remove_reference_t<typename internalType::value_type> internalType;
                if constexpr (std::is_unsigned_v<internalType>)
                {
                    Container container = {};
                    assert(size_t(count) <= container.size());
                    status = true;
                    for (int i = 0; i != count; ++i)
                    {
                        int status_code = WDFunc::CBIndex(parent, widgetName(key, i));

                        if (status_code == -1)
                        {
                            status = false;
                            break;
                        }
                        container.at(i) = internalType(status_code);
                    }
                    configV->setRecordValue({ key, container });
                }
            }
        },
        record.getData());
    return status;
}
