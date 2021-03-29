#include "baseconfig.h"

#include "../models/comboboxdelegate.h"
#include "../module/module_kxx.h"
#include "../widgets/checkboxgroup.h"
#include "../widgets/ipctrl.h"

#include <QHeaderView>
#include <QStandardItem>
BaseConfig::BaseConfig()
{
}

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
                       widget = helper(arg, parent);
                       //                       if (std::is_same_v<decltype(arg), delegate::Item>)
                       //                           widget = helperItem(arg, parent);
                       //                       else
                       //                           widget = helperWidget(arg, parent);
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
                       lyout->addWidget(new CheckBoxGroup(arg.items, parent));
                       widget->setLayout(lyout);
                   },
                   [&](const delegate::QComboBox &arg) {
                       qDebug("QComboBox");
                       // Q_ASSERT(desc.count() == arg.count);
                       widget = new QWidget(parent);
                       QHBoxLayout *lyout = new QHBoxLayout;
                       lyout->addWidget(new QLabel(arg.desc, parent));
                       lyout->addWidget(WDFunc::NewCB2(parent, arg.items));
                       widget->setLayout(lyout);
                   },
               },
        var);
    return widget;
}

QStandardItem *WidgetFactory::createItem(BciNumber key, QWidget *parent)
{
    QStandardItem *item = nullptr;
    auto search = widgetMap.find(key);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return item;
    }
    const auto var = search->second;
    std::visit(overloaded {
                   [&](const auto &arg) {
                       qDebug("DefaultWidget");
                       using namespace delegate;
                   },

                   [&](const delegate::Item &arg) { qDebug("Item"); },
               },
        var);
    return nullptr;
}

template <> QWidget *WidgetFactory::helper(const delegate::Item &arg, QWidget *parent)
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
