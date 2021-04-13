#include "configkxx.h"

#include "../gen/s2.h"
#include "../models/comboboxdelegate.h"
#include "../module/module_kxx.h"
#include "../widgets/ipctrl.h"
#include "../widgets/wd_func.h"
#include "config.h"

#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QVBoxLayout>
ConfigKxx::ConfigKxx()
{
}

void ConfigKxx::SetDefConf()
{
    using namespace DataTypes;
    S2::setRecordValue({ BciNumber::RTerm, float(Bci::DEF_RTERM) });
    S2::setRecordValue({ BciNumber::W100, (Bci::DEF_W100) });
    S2::setRecordValue({ BciNumber::Trele_pred, DWORD(0) });
    S2::setRecordValue({ BciNumber::Trele_alarm, DWORD(0) });

    //..................................................................

    S2::setRecordValue({ BciNumber::MBMaster, BYTE(0) });

    S2::setRecordValue({ BciNumber::MBMab1, BYTE_8t {} });
    S2::setRecordValue({ BciNumber::MBMab2, BYTE_8t {} });
    S2::setRecordValue({ BciNumber::MBMab3, BYTE_8t {} });
    S2::setRecordValue({ BciNumber::MBMab4, BYTE_8t {} });
    //................................................................
    S2::setRecordValue({ BciNumber::IP_ID, BYTE_4t { 172, 16, 29, 12 } });

    S2::setRecordValue({ BciNumber::SNTP_ID, BYTE_4t { 172, 16, 29, 12 } });

    S2::setRecordValue({ BciNumber::GW_ID, BYTE_4t { 172, 16, 29, 1 } });

    S2::setRecordValue({ BciNumber::Mask_ID, BYTE_4t { 255, 255, 252, 0 } });

    S2::setRecordValue({ BciNumber::Port_ID, WORD_4t { 2404, 2405, 502, 502 } });

    S2::setRecordValue({ BciNumber::Baud_ID, DWORD(Bci::DEF_BAUD) });

    S2::setRecordValue({ BciNumber::Parity_ID, BYTE(CommandsMBS::Parity::NoParity) });

    S2::setRecordValue({ BciNumber::stopbit_ID, BYTE(CommandsMBS::StopBits::OneStop) });
    S2::setRecordValue({ BciNumber::adrMB_ID, BYTE(Bci::DEF_ADRMB) });
}

void ConfigKxx::Fill()
{
    using namespace DataTypes;
    int cbidx = 0;
    //....................................................
    SetSPBData<float>(Parent, BciNumber::RTerm);
    SetSPBData<float>(Parent, BciNumber::W100);
    SetSPBData<DWORD>(Parent, BciNumber::Trele_pred);
    SetSPBData<DWORD>(Parent, BciNumber::Trele_alarm);

    //.................................................................

    WDFunc::SetCBIndex(
        Parent, nameByValue(BciNumber::MBMaster), bool(S2::getRecord(BciNumber::MBMaster).value<BYTE>()));

    const auto master1 = S2::getRecord(BciNumber::MBMab1).value<BYTE_8t>();
    const auto master2 = S2::getRecord(BciNumber::MBMab2).value<BYTE_8t>();
    const auto master3 = S2::getRecord(BciNumber::MBMab3).value<BYTE_8t>();
    const auto master4 = S2::getRecord(BciNumber::MBMab4).value<BYTE_8t>();

    const QList arrays { master1, master2, master3, master4 };

    QTableView *tv = Parent->findChild<QTableView *>();
    if (tv == nullptr)
    {
        qDebug("Пустой tv");
        return;
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tv->model());
    model->removeRows(0, model->rowCount());
    for (const auto array : arrays)
    {
        const auto *master = reinterpret_cast<const Bci::ModbusItem *>(&array);
        QList<QStandardItem *> row {
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
        model->appendRow(row);
        tv->adjustSize();
    }
    //.................................................

    const auto StrPort = S2::getRecord(BciNumber::Port_ID).value<WORD_4t>();
    WDFunc::SetLEData(Parent, nameByValue(BciNumber::Port_ID), QString::number(StrPort.front()));

    WDFunc::SetIPCtrlData(Parent, nameByValue(BciNumber::IP_ID), S2::getRecord(BciNumber::IP_ID).value<BYTE_4t>());

    WDFunc::SetIPCtrlData(Parent, nameByValue(BciNumber::SNTP_ID), S2::getRecord(BciNumber::SNTP_ID).value<BYTE_4t>());

    WDFunc::SetIPCtrlData(Parent, nameByValue(BciNumber::GW_ID), S2::getRecord(BciNumber::GW_ID).value<BYTE_4t>());

    WDFunc::SetIPCtrlData(Parent, nameByValue(BciNumber::Mask_ID), S2::getRecord(BciNumber::Mask_ID).value<BYTE_4t>());

    const auto StrBaud = S2::getRecord(BciNumber::Baud_ID).value<DWORD>();
    cbidx = m_baudList.indexOf(QString::number(StrBaud));
    WDFunc::SetCBIndex(Parent, nameByValue(BciNumber::Baud_ID), cbidx != -1 ? cbidx : 0);

    const auto parity = S2::getRecord(BciNumber::Parity_ID).value<BYTE>();
    WDFunc::SetCBIndex(Parent, nameByValue(BciNumber::Parity_ID), parity > 2 ? 0 : parity);
    const auto stopbit = S2::getRecord(BciNumber::stopbit_ID).value<BYTE>();
    WDFunc::SetCBIndex(Parent, nameByValue(BciNumber::stopbit_ID), stopbit);
    WDFunc::SetSPBData(Parent, nameByValue(BciNumber::adrMB_ID), S2::getRecord(BciNumber::adrMB_ID).value<BYTE>());
}

void ConfigKxx::FillBack() const
{
    //.......................................................................
    using namespace DataTypes;
    SPBDataS2<float>(Parent, BciNumber::RTerm);
    SPBDataS2<float>(Parent, BciNumber::W100);
    SPBDataS2<DWORD>(Parent, BciNumber::Trele_pred);
    SPBDataS2<DWORD>(Parent, BciNumber::Trele_alarm);

    //..................................................................

    int cbidx = WDFunc::CBIndex(Parent, nameByValue(BciNumber::MBMaster));
    Q_ASSERT(cbidx != -1);
    S2::setRecordValue({ BciNumber::MBMaster, BYTE(cbidx) });

    QTableView *tv = Parent->findChild<QTableView *>();
    if (tv == nullptr)
    {
        qDebug("Пустой tv");
        return;
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tv->model());
    for (int r = 0; r < model->rowCount(); ++r)
    {
        Bci::ModbusItem master;

        for (int c = 0; c < model->columnCount(); ++c)
        {

            QModelIndex index = model->index(r, c);
            QVariant value = model->data(index);

            bool status = false;
            auto data = value.toUInt(&status);
            switch (c)
            {
            case ModbusColumns::SensorType:
            {
                master.typedat = Bci::SensorType(status ? data : 0);
                break;
            }
            case ModbusColumns::BaudRate:
            {
                master.parport.baud = CommandsMBS::BaudRate(status ? data : 0);
                break;
            }
            case ModbusColumns::Parity:
            {
                master.parport.parity = CommandsMBS::Parity(status ? data : 0);
                break;
            }
            case ModbusColumns::StopBits:
            {
                master.parport.stop = CommandsMBS::StopBits(status ? data : 0);
                break;
            }
            case ModbusColumns::Timeout:
            {
                master.per = status ? data : 0;
                break;
            }
            case ModbusColumns::Address:
            {
                master.adr = status ? data : 0;
                break;
            }
            case ModbusColumns::FuncCode:
            {
                master.type.reg = CommandsMBS::Commands(status ? data : 0);
                break;
            }
            case ModbusColumns::DataType:
            {
                master.type.dat = CommandsMBS::TypeId(status ? data : 0);
                break;
            }
            case ModbusColumns::Register:
            {
                master.reg = (status ? data : 0);
                break;
            }
            default:
                break;
            }
        }
        BYTE_8t masterBuffer = *reinterpret_cast<BYTE_8t *>(&master);
        S2::setRecordValue({ BciNumber::MBMab1 + r, masterBuffer });
    }

    //................................................................

    S2::setRecordValue({ BciNumber::IP_ID, WDFunc::IPCtrlData(Parent, nameByValue(BciNumber::IP_ID)) });

    S2::setRecordValue({ BciNumber::SNTP_ID, WDFunc::IPCtrlData(Parent, nameByValue(BciNumber::SNTP_ID)) });

    S2::setRecordValue({ BciNumber::GW_ID, WDFunc::IPCtrlData(Parent, nameByValue(BciNumber::GW_ID)) });

    S2::setRecordValue({ BciNumber::Mask_ID, WDFunc::IPCtrlData(Parent, nameByValue(BciNumber::Mask_ID)) });

    QString StrPort = WDFunc::LEData(Parent, nameByValue(BciNumber::Port_ID));
    WORD_4t portArray {};
    portArray.at(0) = StrPort.toInt();
    S2::setRecordValue({ BciNumber::Port_ID, portArray });

    cbidx = WDFunc::CBIndex(Parent, nameByValue(BciNumber::Baud_ID));
    S2::setRecordValue({ BciNumber::Baud_ID, DWORD(m_baudList.at(cbidx).toInt()) });

    cbidx = WDFunc::CBIndex(Parent, nameByValue(BciNumber::Parity_ID));
    S2::setRecordValue({ BciNumber::Parity_ID, BYTE(CommandsMBS::Parity(cbidx)) });

    cbidx = WDFunc::CBIndex(Parent, nameByValue(BciNumber::stopbit_ID));
    S2::setRecordValue({ BciNumber::stopbit_ID, BYTE(CommandsMBS::StopBits(cbidx)) });
    S2::setRecordValue({ BciNumber::adrMB_ID, WDFunc::SPBData<BYTE>(Parent, nameByValue(BciNumber::adrMB_ID)) });
}

QWidget *ConfigKxx::ComParam(QWidget *parent)
{
    using namespace DataTypes;
    Parent = parent;
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->setColumnStretch(2, 50);

    int row = 7;

    glyout->addWidget(WDFunc::NewLBL2(parent, "IP адрес устройства:"), row, textColumn, 1, 1, Qt::AlignLeft);
    auto ipControl = new IPCtrl(parent);
    ipControl->setObjectName(nameByValue(BciNumber::IP_ID));
    glyout->addWidget(ipControl, row, valueColumn, 1, 1, Qt::AlignLeft);

    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Маска:"), row, textColumn, 1, 1, Qt::AlignLeft);
    ipControl = new IPCtrl(parent);
    ipControl->setObjectName(nameByValue(BciNumber::Mask_ID));
    glyout->addWidget(ipControl, row, valueColumn, 1, 1, Qt::AlignLeft);

    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Шлюз:"), row, textColumn, 1, 1, Qt::AlignLeft);
    ipControl = new IPCtrl(parent);
    ipControl->setObjectName(nameByValue(BciNumber::GW_ID));
    glyout->addWidget(ipControl, row, valueColumn, 1, 1, Qt::AlignLeft);

    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Порт протокола 104:"), row, textColumn, 1, 1, Qt::AlignLeft);
    glyout->addWidget(
        WDFunc::NewLE2(parent, nameByValue(BciNumber::Port_ID), {}), row, valueColumn, 1, 1, Qt::AlignLeft);

    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес SNTP сервера:"), row, textColumn, 1, 1, Qt::AlignLeft);
    ipControl = new IPCtrl(parent);
    ipControl->setObjectName(nameByValue(BciNumber::SNTP_ID));
    glyout->addWidget(ipControl, row, valueColumn, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    vlyout1->addLayout(vlyout2);

    w->setLayout(vlyout1);

    return w;
}

inline QWidget *createModbusView(QWidget *parent)
{
    QTableView *tableView = new QTableView(parent);
    tableView->verticalScrollBar()->setDisabled(true);
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

QWidget *ConfigKxx::ModbusWidget(QWidget *parent)
{
    Parent = parent;
    QWidget *w = new QWidget;
    QStackedWidget *qswt = new QStackedWidget;
    qswt->setObjectName("qswt");
    QVBoxLayout *vlyout1 = new QVBoxLayout;

    QGroupBox *gb = new QGroupBox;
    QGridLayout *glyout = new QGridLayout;

    QStringList cbl;
    glyout->setColumnStretch(1, 20);

    int row = 0;

    QLabel *lbl = new QLabel("Modbus: ");
    glyout->addWidget(lbl, row, textColumn, 1, 1, Qt::AlignLeft);
    QStringList dopcbl = QStringList { "slave", "master" };
    auto *dopcb = WDFunc::NewCB2(parent, nameByValue(BciNumber::MBMaster), dopcbl);

    QObject::connect(
        dopcb, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) { ChangeModbusGUI(index); });
    glyout->addWidget(dopcb, row, valueColumn, 1, 1);
    row++;

    QVBoxLayout *vlyout2 = new QVBoxLayout;
    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройки ModBus");

    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row++;
    lbl = WDFunc::NewLBL2(parent, "Скорость RS485 интерфейса:");
    glyout->addWidget(lbl, row, textColumn, 1, 1);
    auto *cb = WDFunc::NewCB2(parent, nameByValue(BciNumber::Baud_ID), m_baudList);

    glyout->addWidget(cb, row, valueColumn, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Чётность:"), row, textColumn, 1, 1);
    cbl = QStringList { "NoParity", "EvenParity", "OddParity" };
    cb = WDFunc::NewCB2(parent, nameByValue(BciNumber::Parity_ID), cbl);
    glyout->addWidget(cb, row, valueColumn, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Количество стоповых битов:"), row, textColumn);
    cbl = QStringList { "1", "2" };
    cb = WDFunc::NewCB2(parent, nameByValue(BciNumber::stopbit_ID), cbl);
    glyout->addWidget(cb, row, valueColumn, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес устройства для Modbus:"), row, textColumn);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::adrMB_ID), 1, 254, 0), row, valueColumn);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    qswt->addWidget(gb);
    WidgetList.append(gb);

    gb = new QGroupBox("Настройки ModBus");

    vlyout2 = new QVBoxLayout;

    vlyout2->addWidget(createModbusView(parent));

    gb->setLayout(vlyout2);
    qswt->addWidget(gb);

    WidgetList.append(gb);

    vlyout1->addWidget(qswt);

    w->setLayout(vlyout1);

    return w;
}

QWidget *ConfigKxx::VariousWidget(QWidget *parent)
{
    Parent = parent;
    QWidget *w = new QWidget;
    QGroupBox *gb = new QGroupBox();
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "Номинальное сопротивление термометра при 0 град.С:"));
    hlyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::RTerm), 0, 10000, 0));
    vlyout2->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "Температурный коэффициент термометра:"));
    hlyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::W100), 0, 10000, 3));
    vlyout2->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "Задержка срабатывания реле предупредительной сигнализации:"));
    hlyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::Trele_pred), 0, 10000, 0));
    vlyout2->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "Задержка срабатывания реле аварийной сигнализации:"));
    hlyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::Trele_alarm), 0, 10000, 0));
    vlyout2->addLayout(hlyout);

    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    w->setLayout(vlyout1);

    return w;
}

QObject *ConfigKxx::getParent() const
{
    return Parent;
}

void ConfigKxx::setParent(QObject *value)
{
    Parent = value;
}

void ConfigKxx::ChangeModbusGUI(int num)
{
    using namespace DataTypes;
    S2::setRecordValue({ BciNumber::MBMaster, BYTE(num) });
    QStackedWidget *QSWT = Parent->findChild<QStackedWidget *>("qswt");

    if (QSWT != nullptr)
        QSWT->setCurrentWidget(WidgetList.at(num));
}
