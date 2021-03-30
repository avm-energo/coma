#include "widgetfactory.h"

#include "../models/comboboxdelegate.h"
#include "../module/module_kxx.h"
#include "../widgets/checkboxgroup.h"
#include "../widgets/ipctrl.h"

#include <QHeaderView>
#include <QStandardItem>
widgetMap WidgetFactory::widgetMap;

QWidget *WidgetFactory::createModbusView(QWidget *parent)
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
    // tableView->setMinimumWidth(tableView->horizontalHeader()->height() * 5);
    return tableView;
}

QWidget *WidgetFactory::createWidget(BciNumber key, QWidget *parent)
{
    QWidget *widget = nullptr;
    auto search = widgetMap.find(key);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return widget;
    }

    const auto var = search->second;
    std::visit(overloaded {
                   [&](const auto &arg) {
                       qDebug("DefaultWidget");
                       using namespace delegate;
                       widget = helper(arg, parent, key);
                   },
                   [&](const delegate::DoubleSpinBoxGroup &arg) {
                       qDebug("DoubleSpinBoxGroupWidget");
                       widget = new QWidget(parent);
                       QHBoxLayout *lyout = new QHBoxLayout;
                       lyout->addWidget(new QLabel(arg.desc, parent));
                       lyout->addWidget(
                           WDFunc::NewSPBG(parent, QString::number(key), arg.count, arg.min, arg.max, arg.decimals));
                       widget->setLayout(lyout);
                   },
                   [&](const delegate::DoubleSpinBoxWidget &arg) {
                       qDebug("DoubleSpinBoxWidget");
                       widget = new QWidget(parent);
                       QHBoxLayout *lyout = new QHBoxLayout;
                       lyout->addWidget(new QLabel(arg.desc, parent));
                       lyout->addWidget(WDFunc::NewSPB2(parent, QString::number(key), arg.min, arg.max, arg.decimals));
                       widget->setLayout(lyout);
                   },
                   [&](const delegate::CheckBoxGroup &arg) {
                       qDebug("CheckBoxGroupWidget");
                       // Q_ASSERT(desc.count() == arg.count);
                       widget = new QWidget(parent);
                       QHBoxLayout *lyout = new QHBoxLayout;
                       lyout->addWidget(new QLabel(arg.desc, parent));
                       auto group = new CheckBoxGroup(arg.items, parent);
                       group->setObjectName(QString::number(key));
                       lyout->addWidget(group);
                       widget->setLayout(lyout);
                   },
                   [&](const delegate::QComboBox &arg) {
                       qDebug("QComboBox");
                       // Q_ASSERT(desc.count() == arg.count);
                       widget = new QWidget(parent);
                       QHBoxLayout *lyout = new QHBoxLayout;
                       lyout->addWidget(new QLabel(arg.desc, parent));
                       lyout->addWidget(WDFunc::NewCB2(parent, QString::number(key), arg.items));
                       widget->setLayout(lyout);
                   },
               },
        var);
    return widget;
}

static bool fillBackIpCtrl(BciNumber key, const QWidget *parent)
{
    auto widget = parent->findChild<IPCtrl *>(QString::number(key));
    if (!widget)
        return false;
    S2::setRecordValue({ key, widget->getIP() });
    return true;
}

static bool fillBackCheckBox(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto widget = parent->findChild<QCheckBox *>(QString::number(key));
    if (!widget)
        return status;
    bool state = widget->isChecked();
    auto record = S2::getRecord(key);
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
    return status;
}

static bool fillBackLineEdit(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto widget = parent->findChild<QLineEdit *>(QString::number(key));
    if (!widget)
        return status;
    const QString text = widget->text();
    auto record = S2::getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (!std::is_container<internalType>())
            {
                const auto value = QVariant(text).value<internalType>();
                record.setData(value);
                status = true;
            }
            if constexpr (std::is_container<internalType>())
                if constexpr (!std::is_container<typename internalType::value_type>())
                {
                    const auto value = QVariant(text).value<typename internalType::value_type>();
                    arg.at(0) = value;
                    record.setData(arg);
                    status = true;
                }
        },
        record.getData());
    S2::setRecordValue(record);
    return status;
}

static bool fillBackSPBG(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto record = S2::getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (std::is_container<internalType>())
                if constexpr (sizeof(typename internalType::value_type) != 1
                    && !std::is_container<typename internalType::value_type>())
                {
                    internalType buffer {};
                    status = WDFunc::SPBGData(parent, QString::number(key), buffer);
                    if (status)
                        S2::setRecordValue({ key, buffer });
                }
        },
        record.getData());
    return status;
}

static bool fillBackSPB(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto record = S2::getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (!std::is_container<internalType>())
            {
                auto buffer = WDFunc::SPBData<internalType>(parent, QString::number(key));
                S2::setRecordValue({ key, buffer });
                status = true;
            }
        },
        record.getData());
    return status;
}

static bool fillBackChBG(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto record = S2::getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (!std::is_container<internalType>())
                if constexpr (std::is_integral_v<internalType>)
                {
                    internalType buffer = 0;
                    status = WDFunc::ChBGData(parent, QString::number(key), buffer);
                    if (status)
                        S2::setRecordValue({ key, buffer });
                }
        },
        record.getData());
    return status;
}

static bool fillBackComboBox(BciNumber key, const QWidget *parent)
{
    bool status = false;
    auto record = S2::getRecord(key);
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            qDebug() << typeid(arg).name();
            if constexpr (std::is_unsigned_v<internalType>)
            {
                internalType buffer = 0;
                int status_code = WDFunc::CBIndex(parent, QString::number(key));
                if (status_code == -1)
                    return;
                // status = WDFunc::ChBData(parent, QString::number(key), buffer);
                //   if (status)
                //     S2::setRecordValue({ key, buffer });
            }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBack(BciNumber key, const QWidget *parent)
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
                           break;
                           Q_ASSERT(false && "False type");
                       }
                   },
                   [&](const delegate::DoubleSpinBoxGroup &arg) {
                       qDebug("DoubleSpinBoxGroupWidget");
                       status = fillBackSPBG(key, parent);
                   },
                   [&](const delegate::DoubleSpinBoxWidget &arg) {
                       qDebug("DoubleSpinBoxWidget");
                       status = fillBackSPB(key, parent);
                   },
                   [&](const delegate::CheckBoxGroup &arg) {
                       qDebug("CheckBoxGroupWidget");
                       status = fillBackChBG(key, parent);
                   },
                   [&](const delegate::QComboBox &arg) {
                       qDebug("QComboBox");
                       status = fillBackComboBox(key, parent);
                   },
                   [&](const delegate::Item &arg) {
                       qDebug("Item");

                       const QString widgetName(QString::number(arg.type.hash()) + QString::number(arg.parent));
                       auto tableView = parent->findChild<QTableView *>(widgetName);
                       if (!tableView)
                           return;
                       auto model = qobject_cast<QStandardItemModel *>(tableView->model());
                       if (!model)
                           return;
                   },
               },
        var);
    return status;
}

template <>
QList<QStandardItem *> WidgetFactory::createItem(BciNumber key, const DataTypes::BYTE_8t &value, const QWidget *parent)
{
    QList<QStandardItem *> items {};
    auto search = widgetMap.find(key);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return items;
    }

    const auto var = search->second;
    std::visit(overloaded {
                   [&](const auto &arg) {
                       qDebug("DefaultWidget");
                       using namespace delegate;
                   },

                   [&](const delegate::Item &arg) {
                       qDebug("Item");
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

template <> QWidget *WidgetFactory::helper(const delegate::Item &arg, QWidget *parent, BciNumber key)
{
    QWidget *widget = nullptr;
    switch (arg.itemType)
    {
    case delegate::ItemType::ModbusItem:
    {
        const QString widgetName(QString::number(arg.type.hash()) + QString::number(arg.parent));
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
