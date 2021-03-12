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
#include <QMetaEnum>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QVBoxLayout>
ConfigKxx::ConfigKxx(QObject *parent) : QObject(parent)
{
    // setConfig();
}

void ConfigKxx::setConfig()
{

    Config::removeFotter();

    S2::config.append({ BciNumber::RTerm, sizeof(TempConf.RTerm), &TempConf.RTerm });
    S2::config.append({ BciNumber::W100, sizeof(TempConf.W100), &TempConf.W100 });

    S2::config.append({ BciNumber::Trele_pred, sizeof(StrTrele.Trele_pred), &StrTrele.Trele_pred });
    S2::config.append({ BciNumber::Trele_alarm, sizeof(StrTrele.Trele_alarm), &StrTrele.Trele_alarm });

    S2::config.append({ BciNumber::MBMaster, sizeof(StrModBus.MBMaster), &StrModBus.MBMaster });
    S2::config.append({ BciNumber::MBMab1, sizeof(StrModBus.MBMab1), &StrModBus.MBMab1 });
    S2::config.append({ BciNumber::MBMab2, sizeof(StrModBus.MBMab2), &StrModBus.MBMab2 });
    S2::config.append({ BciNumber::MBMab3, sizeof(StrModBus.MBMab3), &StrModBus.MBMab3 });
    S2::config.append({ BciNumber::MBMab4, sizeof(StrModBus.MBMab4), &StrModBus.MBMab4 });

    S2::config.append({ BciNumber::IP_ID, sizeof(Com_param.IP), &Com_param.IP });
    S2::config.append({ BciNumber::Mask_ID, sizeof(Com_param.Mask), &Com_param.Mask });
    S2::config.append({ BciNumber::GW_ID, sizeof(Com_param.GateWay), &Com_param.GateWay });
    S2::config.append({ BciNumber::Port_ID, sizeof(Com_param.Port), &Com_param.Port });
    S2::config.append({ BciNumber::SNTP_ID, sizeof(Com_param.SNTP), &Com_param.SNTP });
    S2::config.append({ BciNumber::Baud_ID, sizeof(Com_param.Baud), &Com_param.Baud });
    S2::config.append({ BciNumber::Parity_ID, sizeof(Com_param.Parity), &Com_param.Parity });
    S2::config.append({ BciNumber::stopbit_ID, sizeof(Com_param.Stopbit), &Com_param.Stopbit });
    S2::config.append({ BciNumber::adrMB_ID, sizeof(Com_param.adrMB), &Com_param.adrMB });

    S2::config.append({ 0xFFFFFFFF, 0, nullptr });
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
    WDFunc::SetSPBData(ParentSetupBl, NAMEOF(TempConf.RTerm), S2::getRecord(BciNumber::RTerm).value<float>());
    WDFunc::SetSPBData(ParentSetupBl, NAMEOF(TempConf.W100), S2::getRecord(BciNumber::W100).value<float>());
    WDFunc::SetSPBData(ParentSetupBl, NAMEOF(StrTrele.Trele_pred), S2::getRecord(BciNumber::Trele_pred).value<DWORD>());
    WDFunc::SetSPBData(
        ParentSetupBl, NAMEOF(StrTrele.Trele_alarm), S2::getRecord(BciNumber::Trele_alarm).value<DWORD>());

    //.................................................................

    WDFunc::SetCBIndex(ParentMB, NAMEOF(StrModBus.MBMaster), bool(S2::getRecord(BciNumber::MBMaster).value<BYTE>()));

    const auto master1 = S2::getRecord(BciNumber::MBMab1).value<BYTE_8t>();
    const auto master2 = S2::getRecord(BciNumber::MBMab2).value<BYTE_8t>();
    const auto master3 = S2::getRecord(BciNumber::MBMab3).value<BYTE_8t>();
    const auto master4 = S2::getRecord(BciNumber::MBMab4).value<BYTE_8t>();

    const QList arrays { master1, master2, master3, master4 };

    QTableView *tv = ParentMB->findChild<QTableView *>();
    if (tv == nullptr)
    {
        qDebug("Пустой tv");
        return;
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tv->model());
    model->removeRows(0, model->rowCount());
    for (const auto array : arrays)
    {
        const auto *master = reinterpret_cast<const Bci::ABMAST *>(&array);
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
    }
    //.................................................

    const auto StrPort = S2::getRecord(BciNumber::Port_ID).value<WORD_4t>();
    WDFunc::SetLEData(ParentSetup, "Port_ID", QString::number(StrPort.front()));

    WDFunc::SetIPCtrlData(ParentSetup, "IP_ID", S2::getRecord(BciNumber::IP_ID).value<BYTE_4t>());

    WDFunc::SetIPCtrlData(ParentSetup, "SNTP_ID", S2::getRecord(BciNumber::SNTP_ID).value<BYTE_4t>());

    WDFunc::SetIPCtrlData(ParentSetup, "GW_ID", S2::getRecord(BciNumber::GW_ID).value<BYTE_4t>());

    WDFunc::SetIPCtrlData(ParentSetup, "Mask_ID", S2::getRecord(BciNumber::Mask_ID).value<BYTE_4t>());

    const auto StrBaud = S2::getRecord(BciNumber::Baud_ID).value<DWORD>();
    cbidx = m_baudList.indexOf(QString::number(StrBaud));
    WDFunc::SetCBIndex(ParentSetup, "Baud_ID", cbidx != -1 ? cbidx : 0);

    const auto parity = S2::getRecord(BciNumber::Parity_ID).value<BYTE>();
    WDFunc::SetCBIndex(ParentSetup, "Parity_ID", parity > 2 ? 0 : parity);
    const auto stopbit = S2::getRecord(BciNumber::stopbit_ID).value<BYTE>();
    WDFunc::SetCBIndex(ParentSetup, "Stopbit_ID", stopbit);
    WDFunc::SetSPBData(ParentSetup, "adrMB_ID", S2::getRecord(BciNumber::adrMB_ID).value<BYTE>());
}

void ConfigKxx::FillBack()
{
    //.......................................................................
    using namespace DataTypes;
    S2::setRecordValue({ BciNumber::RTerm, WDFunc::SPBData<float>(ParentSetupBl, NAMEOF(TempConf.RTerm)) });
    S2::setRecordValue({ BciNumber::W100, WDFunc::SPBData<float>(ParentSetupBl, NAMEOF(TempConf.W100)) });
    S2::setRecordValue({ BciNumber::Trele_pred, WDFunc::SPBData<DWORD>(ParentSetupBl, NAMEOF(StrTrele.Trele_pred)) });
    S2::setRecordValue({ BciNumber::Trele_alarm, WDFunc::SPBData<DWORD>(ParentSetupBl, NAMEOF(StrTrele.Trele_alarm)) });

    //..................................................................

    int cbidx = WDFunc::CBIndex(ParentMB, NAMEOF(StrModBus.MBMaster));
    Q_ASSERT(cbidx != -1);
    S2::setRecordValue({ BciNumber::MBMaster, BYTE(cbidx) });

    std::vector<Bci::ABMAST> masters { 4, Bci::ABMAST() };

    QTableView *tv = ParentMB->findChild<QTableView *>();
    if (tv == nullptr)
    {
        qDebug("Пустой tv");
        return;
    }
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tv->model());
    for (int r = 0; r < model->rowCount(); ++r)
    {
        Bci::ABMAST master;

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

    S2::setRecordValue({ BciNumber::IP_ID, WDFunc::IPCtrlData(ParentSetup, "IP_ID") });

    S2::setRecordValue({ BciNumber::SNTP_ID, WDFunc::IPCtrlData(ParentSetup, "SNTP_ID") });

    S2::setRecordValue({ BciNumber::GW_ID, WDFunc::IPCtrlData(ParentSetup, "GW_ID") });

    S2::setRecordValue({ BciNumber::Mask_ID, WDFunc::IPCtrlData(ParentSetup, "Mask_ID") });

    QString StrPort = WDFunc::LEData(ParentSetup, "Port_ID");
    WORD_4t portArray {};
    portArray.at(0) = StrPort.toInt();
    S2::setRecordValue({ BciNumber::Port_ID, portArray });

    cbidx = WDFunc::CBIndex(ParentSetup, "Baud_ID");
    S2::setRecordValue({ BciNumber::Baud_ID, DWORD(m_baudList.at(cbidx).toInt()) });

    cbidx = WDFunc::CBIndex(ParentSetup, "Parity_ID");
    S2::setRecordValue({ BciNumber::Parity_ID, BYTE(CommandsMBS::Parity(cbidx)) });

    cbidx = WDFunc::CBIndex(ParentSetup, "Stopbit_ID");
    S2::setRecordValue({ BciNumber::stopbit_ID, BYTE(CommandsMBS::StopBits(cbidx)) });
    S2::setRecordValue({ BciNumber::adrMB_ID, WDFunc::SPBData<BYTE>(ParentSetup, "adrMB_ID") });
}

QWidget *ConfigKxx::ComParam(QWidget *parent)
{
    using namespace DataTypes;
    ParentSetup = parent;
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->setColumnStretch(2, 50);

    int row = 7;

    glyout->addWidget(WDFunc::NewLBL2(parent, "IP адрес устройства:"), row, 0, 1, 1, Qt::AlignLeft);
    auto ipControl = new IPCtrl;
    ipControl->setObjectName("IP_ID");
    glyout->addWidget(ipControl, row, 1, 1, 1, Qt::AlignLeft);

    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Маска:"), row, 0, 1, 1, Qt::AlignLeft);
    ipControl = new IPCtrl;
    ipControl->setObjectName("Mask_ID");
    glyout->addWidget(ipControl, row, 1, 1, 1, Qt::AlignLeft);

    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Шлюз:"), row, 0, 1, 1, Qt::AlignLeft);
    ipControl = new IPCtrl;
    ipControl->setObjectName("GW_ID");
    glyout->addWidget(ipControl, row, 1, 1, 1, Qt::AlignLeft);

    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Порт протокола 104:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE2(parent, "Port_ID", {}), row, 1, 1, 1, Qt::AlignLeft);

    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес SNTP сервера:"), row, 0, 1, 1, Qt::AlignLeft);
    ipControl = new IPCtrl;
    ipControl->setObjectName("SNTP_ID");
    glyout->addWidget(ipControl, row, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    vlyout1->addLayout(vlyout2);

    w->setLayout(vlyout1);

    return w;
}

QWidget *ConfigKxx::ModbusWidget(QWidget *parent)
{
    ParentMB = parent;
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
    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList dopcbl = QStringList { "slave", "master" };
    auto *dopcb = WDFunc::NewCB2(parent, NAMEOF(StrModBus.MBMaster), dopcbl);

    connect(dopcb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConfigKxx::ChangeModbusGUI);
    glyout->addWidget(dopcb, row, 1, 1, 1);
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
    glyout->addWidget(lbl, row, 0, 1, 1);
    auto *cb = WDFunc::NewCB2(parent, "Baud_ID", m_baudList);

    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Чётность:"), row, 0, 1, 1);
    cbl = QStringList { "NoParity", "EvenParity", "OddParity" };
    cb = WDFunc::NewCB2(parent, "Parity_ID", cbl);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Количество стоповых битов:"), row, 0, 1, 1);
    cbl = QStringList { "1", "2" };
    cb = WDFunc::NewCB2(parent, "Stopbit_ID", cbl);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес устройства для Modbus:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(parent, "adrMB_ID", 1, 254, 0), row, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    qswt->addWidget(gb);
    WidgetList.append(gb);

    gb = new QGroupBox("Настройки ModBus");

    vlyout2 = new QVBoxLayout;

    QTableView *tableView = new QTableView(parent);
    tableView->setSelectionMode(QAbstractItemView::NoSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableView->setShowGrid(false);
    tableView->setStyleSheet("QTableView {background-color: transparent;}");
    tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    ComboBoxDelegate *comboBoxdelegate = new ComboBoxDelegate({ "нет", "тип 1", "тип 2", "тип 3" }, tableView);
    tableView->setItemDelegateForColumn(0, comboBoxdelegate);

    comboBoxdelegate = new ComboBoxDelegate(m_baudList, tableView);
    tableView->setItemDelegateForColumn(1, comboBoxdelegate);

    comboBoxdelegate = new ComboBoxDelegate({ "нет", "even", "odd" }, tableView);
    tableView->setItemDelegateForColumn(2, comboBoxdelegate);

    comboBoxdelegate = new ComboBoxDelegate({ "1", "2" }, tableView);
    tableView->setItemDelegateForColumn(3, comboBoxdelegate);

    SpinBoxDelegate *spinBoxDelegate
        = new SpinBoxDelegate(0, std::numeric_limits<decltype(Bci::ABMAST::per)>::max(), tableView);
    tableView->setItemDelegateForColumn(4, spinBoxDelegate);

    spinBoxDelegate = new SpinBoxDelegate(0, std::numeric_limits<decltype(Bci::ABMAST::adr)>::max(), tableView);
    tableView->setItemDelegateForColumn(5, spinBoxDelegate);

    const QStringList funcs { "Coils", "Status", "Holding", "Input" };
    comboBoxdelegate = new ComboBoxDelegate(funcs, tableView);
    comboBoxdelegate->setOffset(1);
    tableView->setItemDelegateForColumn(6, comboBoxdelegate);

    const QStringList types { "Uint16", "Int16", "Bool", "Uint32", "Float" };
    comboBoxdelegate = new ComboBoxDelegate(types, tableView);
    tableView->setItemDelegateForColumn(7, comboBoxdelegate);

    spinBoxDelegate = new SpinBoxDelegate(0, std::numeric_limits<decltype(Bci::ABMAST::reg)>::max(), tableView);
    tableView->setItemDelegateForColumn(8, spinBoxDelegate);

    QStandardItemModel *model = new QStandardItemModel(tableView);
    model->setHorizontalHeaderLabels(m_header);
    tableView->setModel(model);

    for (int column = 0; column < model->columnCount(); column++)
    {
        // NOTE Ужасный костыль
        int width = tableView->horizontalHeader()->fontMetrics().horizontalAdvance(m_header.at(column)) * 1.5;
        tableView->setColumnWidth(column, width);
    }

    vlyout2->addWidget(tableView);

    gb->setLayout(vlyout2);
    qswt->addWidget(gb);

    WidgetList.append(gb);

    vlyout1->addWidget(qswt);

    w->setLayout(vlyout1);

    return w;
}

QWidget *ConfigKxx::VariousWidget(QWidget *parent)
{
    ParentSetupBl = parent;
    QWidget *w = new QWidget;
    QGroupBox *gb = new QGroupBox();
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "Номинальное сопротивление термометра при 0 град.С:"));
    hlyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(TempConf.RTerm), 0, 10000, 0));
    vlyout2->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "Температурный коэффициент термометра:"));
    hlyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(TempConf.W100), 0, 10000, 3));
    vlyout2->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "Задержка срабатывания реле предупредительной сигнализации:"));
    hlyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(StrTrele.Trele_pred), 0, 10000, 0));
    vlyout2->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "Задержка срабатывания реле аварийной сигнализации:"));
    hlyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(StrTrele.Trele_alarm), 0, 10000, 0));
    vlyout2->addLayout(hlyout);

    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    w->setLayout(vlyout1);

    return w;
}

void ConfigKxx::ChangeModbusGUI(int num)
{
    using namespace DataTypes;
    S2::setRecordValue({ BciNumber::MBMaster, BYTE(num) });
    QStackedWidget *QSWT = ParentMB->findChild<QStackedWidget *>("qswt");

    if (QSWT != nullptr)
        QSWT->setCurrentWidget(WidgetList.at(num));
}
