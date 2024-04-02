#include "widgets/widgetfactory.h"

#include <QHeaderView>
#include <QStandardItem>
#include <ctti/type_id.hpp>
#include <models/comboboxdelegate.h>
#include <widgets/checkboxgroup.h>
#include <widgets/flowlayout.h>
#include <widgets/ipctrl.h>
#include <widgets/modbusitem.h>

// forward declarations
// helpers for create widget
static QWidget *createModbusView(QWidget *parent);

using namespace ModbusItem;

template <typename T> QWidget *helper(const T &arg, QWidget *parent, quint16 key)
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
    case ctti::unnamed_type_id<GasDensityWidget>().hash():
    {
        auto gasWidget = new GasDensityWidget(parent);
        gasWidget->setObjectName(QString::number(key));
        lyout->addWidget(gasWidget);
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
        qWarning() << "Type not found " << key;
        Q_ASSERT(false && "False type");
        widget->deleteLater();
        widget = nullptr;
        break;
    }
    return widget;
}

/// DON'T REMOVE THIS FUNCTION
template <> QWidget *helper(const config::Item &arg, QWidget *parent, [[maybe_unused]] quint16 key)
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

template <typename T> bool WidgetFactory::fillBackItem(quint16 key, const QWidget *parent, quint16 parentKey) const
{
    const auto mbMasterId = m_s2storage.getIdFor("MBMaster");
    if (parentKey == mbMasterId)
    {
        return fillBackModbus(key, parent, ctti::unnamed_type_id<QTableView>(), parentKey);
    }
    else
    {
        Q_ASSERT(false && "Unsupported type");
    }
    return false;
};

WidgetFactory::WidgetFactory(S2::Configuration &workingConfig, Device::CurrentDevice *device)
    : m_config(workingConfig)
    , m_storage(*device->getConfigStorage())
    , m_s2storage(device->getS2Datamanager()->getStorage())
{
}

QWidget *WidgetFactory::createWidget(quint16 key, QWidget *parent)
{
    QWidget *widget = nullptr;
    auto &widgetMap = m_s2storage.getWidgetMap();
    auto search = widgetMap.find(key);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return widget;
    }

    const auto var = search->second;
    std::visit(
        overloaded {
            [&](const delegate::DoubleSpinBoxGroup &arg) {
                widget = new QWidget(parent);
                QHBoxLayout *lyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                lyout->addWidget(label);

                QWidget *spbGroup;
                if (!arg.items.isEmpty())
                {
                    assert(static_cast<uint32_t>(arg.items.count()) == arg.count);
                    spbGroup = WDFunc::NewSPBG(parent, QString::number(key), arg.items, arg.min, arg.max, arg.decimals);
                }
                else
                    spbGroup = WDFunc::NewSPBG(parent, QString::number(key), arg.count, arg.min, arg.max, arg.decimals);

                lyout->addWidget(spbGroup);
                widget->setLayout(lyout);
            },
            [&](const delegate::DoubleSpinBoxWidget &arg) {
                widget = new QWidget(parent);
                QHBoxLayout *lyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                lyout->addWidget(label);
                lyout->addWidget(WDFunc::NewSPB2(parent, QString::number(key), arg.min, arg.max, arg.decimals));
                widget->setLayout(lyout);
            },
            [&](const delegate::CheckBoxGroup &arg) {
                widget = new QWidget(parent);
                QHBoxLayout *lyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                lyout->addWidget(label);

                auto count = getRealCount(key);
                auto group = new CheckBoxGroup(arg.items, count, parent);
                group->setObjectName(QString::number(key));
                lyout->addWidget(group);
                widget->setLayout(lyout);
            },
            [&](const delegate::QComboBox &arg) {
                widget = new QWidget(parent);
                QHBoxLayout *lyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                lyout->addWidget(label);
                lyout->addWidget(WDFunc::NewCB2(parent, QString::number(key), arg.model));
                widget->setLayout(lyout);
            },
            [&](const delegate::QComboBoxGroup &arg) {
                widget = new QWidget(parent);
                QHBoxLayout *mainLyout = new QHBoxLayout;
                auto label = new QLabel(arg.desc, parent);
                label->setToolTip(arg.toolTip);
                mainLyout->addWidget(label);
                auto count = getRealCount(key);
                FlowLayout *flowLayout = new FlowLayout;
                for (auto i = 0; i != count; ++i)
                {
                    QWidget *w = new QWidget;
                    QHBoxLayout *layout = new QHBoxLayout;
                    layout->addWidget(new QLabel(QString::number(i + 1), parent));
                    layout->addWidget(WDFunc::NewCB2(parent, widgetName(key, i), arg.model));
                    w->setLayout(layout);
                    flowLayout->addWidget(w);
                }
                mainLyout->addLayout(flowLayout);
                widget->setLayout(mainLyout);
            },
            [&](const auto &arg) { widget = helper(arg, parent, key); },
        },
        var);
    return widget;
}

bool WidgetFactory::fillBack(quint16 key, const QWidget *parent) const
{
    bool status = false;
    auto &widgetMap = m_s2storage.getWidgetMap();
    auto search = widgetMap.find(key);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return status;
    }

    const auto var = search->second;
    std::visit(
        overloaded {
            [&]([[maybe_unused]] const delegate::DoubleSpinBoxGroup &arg) { status = fillBackSPBG(key, parent); },
            [&]([[maybe_unused]] const delegate::DoubleSpinBoxWidget &arg) { status = fillBackSPB(key, parent); },
            [&]([[maybe_unused]] const delegate::CheckBoxGroup &arg) { status = fillBackChBG(key, parent); },
            [&](const delegate::QComboBox &arg) { status = fillBackComboBox(key, parent, arg.primaryField); },
            [&](const delegate::QComboBoxGroup &arg) { status = fillBackComboBoxGroup(key, parent, arg.count); },
            [&](const config::Item &arg) {
                auto &record = m_config[key];
                std::visit(
                    [&](auto &&type) {
                        typedef std::remove_reference_t<decltype(type)> internalType;
                        status = fillBackItem<internalType>(key, parent, arg.parent);
                    },
                    record.getData());
            },
            [&](const auto &arg) {
                using namespace delegate;
                switch (arg.type.hash())
                {
                case ctti::unnamed_type_id<IPCtrl>().hash():
                    status = fillBackIpCtrl(key, parent);
                    break;
                case ctti::unnamed_type_id<GasDensityWidget>().hash():
                    status = fillBackGasWidget(key, parent);
                    break;
                case ctti::unnamed_type_id<QCheckBox>().hash():
                    status = fillBackCheckBox(key, parent);
                    break;
                case ctti::unnamed_type_id<QLineEdit>().hash():
                    status = fillBackLineEdit(key, parent);
                    break;
                default:
                    Q_ASSERT(false && "False type");
                    break;
                }
            },
        },
        var);
    return status;
}

bool WidgetFactory::fillCheckBox(const QWidget *parent, quint16 key, bool value)
{
    auto widget = parent->findChild<QCheckBox *>(QString::number(key));
    if (!widget)
        return false;
    widget->setChecked(bool(value));
    return true;
}

bool WidgetFactory::fillGasWidget(const QWidget *parent, quint16 key, const S2::CONF_DENS_3t &value)
{
    auto widget = parent->findChild<GasDensityWidget *>(QString::number(key));
    if (!widget)
        return false;
    widget->fill(value);
    return true;
}

template <>
QList<QStandardItem *> WidgetFactory::createItem(
    quint16 key, const S2::BYTE_8t &value, [[maybe_unused]] const QWidget *parent)
{
    QList<QStandardItem *> items {};
    auto &widgetMap = m_s2storage.getWidgetMap();
    auto search = widgetMap.find(key);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return items;
    }

    const auto var = search->second;
    std::visit(overloaded {
                   [](const auto &_) { Q_UNUSED(_); },
                   [&](const config::Item &arg) {
                       switch (arg.itemType)
                       {
                       case delegate::ItemType::ModbusItem:
                       {
                           const auto *master = reinterpret_cast<const Item *>(&value);
                           items = {
                               (new QStandardItem(QString::number(master->typedat))),        //
                               (new QStandardItem(QString::number(master->parport.baud))),   //
                               (new QStandardItem(QString::number(master->parport.parity))), //
                               (new QStandardItem(QString::number(master->parport.stop))),   //
                               (new QStandardItem(QString::number(master->per))),            //
                               (new QStandardItem(QString::number(master->adr))),            //
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
        = new SpinBoxDelegate(0, std::numeric_limits<decltype(Item::per)>::max(), tableView);
    tableView->setItemDelegateForColumn(4, spinBoxDelegate);

    spinBoxDelegate = new SpinBoxDelegate(0, std::numeric_limits<decltype(Item::adr)>::max(), tableView);
    tableView->setItemDelegateForColumn(5, spinBoxDelegate);

    const QStringList funcs { "Coils", "Status", "Holding", "Input" };
    comboBoxdelegate = new ComboBoxDelegate(funcs, tableView);
    // Modbus функции начинаются с 1
    comboBoxdelegate->setOffset(1);
    tableView->setItemDelegateForColumn(6, comboBoxdelegate);

    const QStringList types { "Uint16", "Int16", "Bool", "Uint32", "Float" };
    comboBoxdelegate = new ComboBoxDelegate(types, tableView);
    tableView->setItemDelegateForColumn(7, comboBoxdelegate);

    spinBoxDelegate = new SpinBoxDelegate(0, std::numeric_limits<decltype(Item::reg)>::max(), tableView);
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

QString WidgetFactory::hashedName(ctti::unnamed_type_id_t type, quint16 key)
{
    return QString::number(type.hash()) + QString::number(key);
}

const QString WidgetFactory::widgetName(int group, int item)
{
    return QString::number(group) + "-" + QString::number(item);
}

/// TODO: ОЧЕНЬ ПЛОХОЕ РЕШЕНИЕ, МАКСИМАЛЬНЫЙ КОСТЫЛЬ
quint16 WidgetFactory::getRealCount(const quint16 key)
{
    auto &widgetMap = m_s2storage.getWidgetMap();
    auto widgetSearch = widgetMap.find(key);
    if (widgetSearch != widgetMap.end())
    {
        quint16 realCount = 0;
        const auto &cfgCountMap = m_storage.getDeviceSettings().getDetailConfigCount();
        auto countSearch = cfgCountMap.find(key);
        if (countSearch != cfgCountMap.end())
        {
            realCount = countSearch.value();
        }
        else
        {
            std::visit(overloaded {
                           [&](const delegate::CheckBoxGroup &val) { realCount = val.count; },
                           [&](const delegate::QComboBoxGroup &val) { realCount = val.count; },
                           [&]([[maybe_unused]] const auto &arg) { realCount = 0; },
                       },
                widgetSearch->second);
        }
        return realCount;
    }
    else
        return 0;
}

bool WidgetFactory::fillBackModbus(
    quint32 id, const QWidget *parent, ctti::unnamed_type_id_t type, quint16 parentKey) const
{
    auto tableView = parent->findChild<QTableView *>(WidgetFactory::hashedName(type, parentKey));

    if (tableView == nullptr)
    {
        qDebug("Пустой tv");
        return false;
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tableView->model());
    // -1 hardcoded as diff between parent element and first child element
    int row = id - parentKey - 1;

    Item master;

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
            master.typedat = SensorType(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::BaudRate:
        {
            master.parport.baud = status ? data : 0;
            break;
        }
        case config::Item::ModbusColumns::Parity:
        {
            master.parport.parity = Parity(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::StopBits:
        {
            master.parport.stop = StopBits(status ? data : 0);
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
            master.func = Modbus::FunctionCode(status ? data : 0);
            break;
        }
        case config::Item::ModbusColumns::DataType:
        {
            master.typedata = TypeId(status ? data : 0);
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

        S2::BYTE_8t masterBuffer = *reinterpret_cast<S2::BYTE_8t *>(&master);
        m_config.setRecord(id, masterBuffer);
    }
    return true;
}

bool WidgetFactory::fillBackIpCtrl(quint32 id, const QWidget *parent) const
{
    auto widget = parent->findChild<IPCtrl *>(QString::number(id));
    if (!widget)
        return false;
    m_config.setRecord(id, widget->getIP());
    return true;
}

bool WidgetFactory::fillBackCheckBox(quint32 id, const QWidget *parent) const
{
    bool status = false;
    auto widget = parent->findChild<QCheckBox *>(QString::number(id));
    if (!widget)
        return status;
    bool state = widget->isChecked();
    auto &record = m_config[id];
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

bool WidgetFactory::fillBackLineEdit(quint32 id, const QWidget *parent) const
{
    bool status = false;
    auto widget = parent->findChild<QLineEdit *>(QString::number(id));
    if (!widget)
        return status;
    const QString text = widget->text();
    auto &record = m_config[id];
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
    return status;
}

bool WidgetFactory::fillBackSPBG(quint32 id, const QWidget *parent) const
{
    bool status = false;
    auto &record = m_config[id];
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (std_ext::is_container<internalType>())
            {
                using container_type = typename internalType::value_type;
                if constexpr (sizeof(container_type) != 1 &&    //
                    !std_ext::is_container<container_type>() && //
                    !std::is_same_v<container_type, S2::CONF_DENS>)
                {
                    internalType buffer {};
                    status = WDFunc::SPBGData(parent, QString::number(id), buffer);
                    if (status)
                        record.setData(buffer);
                }
            }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBackSPB(quint32 id, const QWidget *parent) const
{
    bool status = false;
    auto &record = m_config[id];
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (!std_ext::is_container<internalType>())
            {
                auto buffer = WDFunc::SPBData<internalType>(parent, QString::number(id));
                record.setData(buffer);
                status = true;
            }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBackChBG(quint32 id, const QWidget *parent) const
{
    bool status = false;
    auto &record = m_config[id];
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (!std_ext::is_container<internalType>())
            {
                if constexpr (std::is_unsigned_v<internalType>)
                {
                    internalType buffer = 0;
                    status = WDFunc::ChBGData(parent, QString::number(id), buffer);
                    if (status)
                        record.setData(buffer);
                }
            }

            else if constexpr (std_ext::is_container<internalType>())
            {
                typedef internalType Container;
                typedef std::remove_reference_t<typename internalType::value_type> iType;
                if constexpr (std::is_integral<iType>::value)
                {
                    Container buffer;
                    status = WDFunc::ChBGData(parent, QString::number(id), buffer);
                    if (status)
                        record.setData(buffer);
                }
            }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBackComboBox(quint32 id, const QWidget *parent, delegate::QComboBox::PrimaryField field) const
{
    bool status = false;
    auto &record = m_config[id];
    std::visit(
        [&](auto &&arg) {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            if constexpr (std::is_arithmetic_v<internalType>)
            {
                switch (field)
                {
                case delegate::QComboBox::data:
                {
                    auto buffer = WDFunc::CBData<internalType>(parent, QString::number(id));
                    record.setData(buffer);
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
                        int status_code = WDFunc::CBIndex(parent, QString::number(id));
                        if (status_code == -1)
                            return;
                        record.setData(static_cast<internalType>(status_code));
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

bool WidgetFactory::fillBackComboBoxGroup(quint32 id, const QWidget *parent, int count) const
{
    bool status = false;
    auto &record = m_config[id];
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
                    int status_code = WDFunc::CBIndex(parent, widgetName(id, i));
                    if (status_code == -1)
                    {
                        status = false;
                        break;
                    }
                    bitset.set(i, bool(status_code));
                }
                record.setData(static_cast<internalType>(bitset.to_ullong()));
            }
            else if constexpr (std_ext::is_container<internalType>())
            {
                typedef internalType Container;
                typedef std::remove_reference_t<typename internalType::value_type> iType;
                if constexpr (std::is_unsigned_v<iType>)
                {
                    Container container = {};
                    assert(size_t(count) <= container.size());
                    status = true;
                    for (int i = 0; i != count; ++i)
                    {
                        int status_code = WDFunc::CBIndex(parent, widgetName(id, i));
                        if (status_code == -1)
                        {
                            status = false;
                            break;
                        }
                        container.at(i) = iType(status_code);
                    }
                    record.setData(container);
                }
            }
        },
        record.getData());
    return status;
}

bool WidgetFactory::fillBackGasWidget(quint32 id, const QWidget *parent) const
{
    auto widget = parent->findChild<GasDensityWidget *>(QString::number(id));
    if (!widget)
        return false;
    auto &record = m_config[id];
    record.setData(widget->fillBack());
    return true;
}
