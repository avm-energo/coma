#include "dialogs/connDialogs/IEC104Dialog/interfaceethernetdialog.h"

#include <QtNetwork/QHostAddress>
#include <common/names.h>
#include <dialogs/connDialogs/IEC104Dialog/scanethernetdevicesdialog.h>
#include <libavm-gen/error.h>
#include <libavm-gen/settings.h>
#include <libavm-widgets/emessagebox.h>
#include <libavm-widgets/lefunc.h>
#include <libavm-widgets/pbfunc.h>
#include <libavm-widgets/spbfunc.h>
#include <libavm-widgets/tvfunc.h>

#include <QDebug>
#include <QGroupBox>
#include <QMessageBox>
#include <QSettings>
#include <QStandardItemModel>
#include <QVBoxLayout>

InterfaceEthernetDialog::InterfaceEthernetDialog(QWidget *parent)
    : AbstractInterfaceDialog(parent)
    , m_addWidget(new QGroupBox("Добавление", this))
{
}

InterfaceEthernetDialog::~InterfaceEthernetDialog() noexcept { }

void InterfaceEthernetDialog::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *TVLayout = new QVBoxLayout;
    QHBoxLayout *TVbuttonLayout = new QHBoxLayout;
    QVBoxLayout *addScanLayout = new QVBoxLayout;

    m_tableView = TVFunc::New(this, "", nullptr);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    TVLayout->addWidget(m_tableView);

    QPushButton *removeButton = PBFunc::New(this, "", "Удалить", this, &InterfaceEthernetDialog::deleteInterface);
    QPushButton *cancelButton = PBFunc::New(this, "", "Назад", this, &QDialog::close);

    TVbuttonLayout->addWidget(removeButton);
    TVbuttonLayout->addWidget(cancelButton);
    TVLayout->addLayout(TVbuttonLayout);

    mainLayout->addLayout(TVLayout);

    setupAddWidget();
    addScanLayout->addWidget(m_addWidget);
    addScanLayout->addWidget(new ScanEthernetDevicesDialog(this, this));

    mainLayout->addLayout(addScanLayout);

    setLayout(mainLayout);

    connect(m_tableView, &QTableView::doubleClicked, this, &InterfaceEthernetDialog::setInterface);
    // connect del
}

void InterfaceEthernetDialog::setInterface(QModelIndex index)
{
    auto *mdl = index.model();
    int row = index.row();
    QString name = mdl->data(mdl->index(row, 0)).toString();
    QString ip = mdl->data(mdl->index(row, 1)).toString();
    quint16 port = mdl->data(mdl->index(row, 2)).toUInt();
    quint16 address = mdl->data(mdl->index(row, 3)).toUInt();

    BaseSettings *settings = buildSettings(ip, port, address);
    if (settings == nullptr)
        return;
    apply(settings);

    ConnectionSettings st = wrapSettings(name, settings);
    emit accepted(st);
}

quint16 InterfaceEthernetDialog::defaultPort() const
{
    return Settings::get(SettingsKeys::Iec104::iec104DefaultPort, 2404);
}

quint16 InterfaceEthernetDialog::defaultAddress() const
{
    return Settings::get(SettingsKeys::Iec104::iec104DefaultBsAddress, 205);
}

QString InterfaceEthernetDialog::settingsGroup() const
{
    return "Ethernet";
}

QString InterfaceEthernetDialog::addressLabel() const
{
    return "Адрес БС";
}

QString InterfaceEthernetDialog::addressSettingsKey() const
{
    return "iec104BsAddress";
}

QString InterfaceEthernetDialog::zeroAddressError() const
{
    return "Адрес базовой станции не может быть равен нулю";
}

BaseSettings *InterfaceEthernetDialog::buildSettings(const QString &ip, quint16 port, quint16 address) const
{
    IEC104Settings *settings = new IEC104Settings;
    settings->set("ip", ip);
    settings->set("port", port);
    settings->set("bsAddress", address);
    settings->set("timeout", Settings::get("iec104Timeout", 1000));
    settings->set("reconnectInterval", Settings::get("iec104Reconnect", 1000));
    settings->set("disconnectTimeout", Settings::get("iec104DisconnectTimeout", 5000));
    settings->set("connectTimeout", Settings::get("iec104ConnectTimeout", 5000));
    settings->set("t0", Settings::get("iec104T0", 30));
    settings->set("t1", Settings::get("iec104T1", 15));
    settings->set("t2", Settings::get("iec104T2", 10));
    settings->set("t3", Settings::get("iec104T3", 20));
    settings->set("k", Settings::get("iec104K", 12));
    settings->set("w", Settings::get("iec104W", 8));

    if (!settings->isValid())
    {
        delete settings;
        return nullptr;
    }
    return settings;
}

ConnectionSettings InterfaceEthernetDialog::wrapSettings(const QString &name, BaseSettings *settings) const
{
    return ConnectionSettings { name, qobject_cast<IEC104Settings *>(settings) };
}

void InterfaceEthernetDialog::addInterface() { }

void InterfaceEthernetDialog::acceptedInterface()
{
    if (checkSize())
    {
        EMessageBox::warning(this, "Превышен лимит соединений!");
        return;
    }

    QString name = LEFunc::data(m_addWidget, "nameConnection");

    if (Settings::groups(settingsGroup()).contains(name))
    {
        EMessageBox::error(this, "Соединение с таким именем уже существует");
        return;
    }

    Settings::pushGroup(settingsGroup());

    QString ipStr = QString("%1.%2.%3.%4")
                        .arg(QString::number(SPBFunc::data<int>(m_addWidget, "ipCell_0")),
                            QString::number(SPBFunc::data<int>(m_addWidget, "ipCell_1")),
                            QString::number(SPBFunc::data<int>(m_addWidget, "ipCell_2")),
                            QString::number(SPBFunc::data<int>(m_addWidget, "ipCell_3")));

    quint16 port = SPBFunc::data<quint16>(m_addWidget, "port");
    quint16 address = SPBFunc::data<quint16>(m_addWidget, "BSAdress");

    if (address == 0)
    {
        EMessageBox::error(this, zeroAddressError());
        Settings::popGroup();
        return;
    }

    Settings::pushGroup(name);
    Settings::set("ipAddress", ipStr);
    Settings::set("ipPort", port);
    Settings::set(addressSettingsKey(), address);
    Settings::popGroup();
    Settings::popGroup(); // exit from settingsGroup()

    if (!updateModel())
        qDebug() << Error::GeneralError;
}

void InterfaceEthernetDialog::displayScanResults(const QList<quint32> &hosts)
{
    QStandardItemModel *mdl = qobject_cast<QStandardItemModel *>(m_tableView->model());
    mdl->removeRows(0, mdl->rowCount());
    QString port = QString::number(defaultPort());
    QString address = QString::number(defaultAddress());
    for (const auto &host : hosts)
    {
        QList<QStandardItem *> row { new QStandardItem("AVM"), new QStandardItem(QHostAddress(host).toString()),
            new QStandardItem(port), new QStandardItem(address) };
        mdl->appendRow(row);
    }
}

bool InterfaceEthernetDialog::updateModel()
{
    QStringList headers { "Имя", "IP", "Порт", addressLabel() };
    auto model = static_cast<QStandardItemModel *>(m_tableView->model());
    if (model == nullptr)
        model = new QStandardItemModel(this);
    else
        model->clear();
    model->setHorizontalHeaderLabels(headers);

    Settings::pushGroup(settingsGroup());
    QStringList ethList = Settings::groups();
    for (const auto &item : std::as_const(ethList))
    {
        Settings::pushGroup(item);
        QList<QStandardItem *> items {
            new QStandardItem(item),                                                          //
            new QStandardItem(QString(Settings::get("ipAddress", "127.0.0.1"))),              //
            new QStandardItem(QString(Settings::get("ipPort", defaultPort()))),               //
            new QStandardItem(QString(Settings::get(addressSettingsKey(), defaultAddress()))) //
        };
        model->appendRow(items);
        Settings::popGroup();
    }
    Settings::popGroup();

    m_tableView->setModel(model);
    m_tableView->resizeColumnsToContents();

    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
        &InterfaceEthernetDialog::rowSelected, Qt::UniqueConnection);

    return true;
}

void InterfaceEthernetDialog::rowSelected(const QModelIndex &current)
{
    if (!current.isValid())
        return;

    const auto *mdl = current.model();
    int row = current.row();

    QString name = mdl->data(mdl->index(row, 0)).toString();
    QStringList ipParts = mdl->data(mdl->index(row, 1)).toString().split('.');
    quint16 port = mdl->data(mdl->index(row, 2)).toUInt();
    quint16 bsAddress = mdl->data(mdl->index(row, 3)).toUInt();

    LEFunc::setData(m_addWidget, "nameConnection", name);

    for (int i = 0; i < ipParts.size() && i < 4; ++i)
        SPBFunc::setData(m_addWidget, QString("ipCell_%1").arg(i), ipParts.at(i).toDouble());

    SPBFunc::setData(m_addWidget, "port", port);
    SPBFunc::setData(m_addWidget, "BSAdress", bsAddress);
}

void InterfaceEthernetDialog::deleteInterface()
{
    QString name = m_tableView->currentIndex().siblingAtColumn(0).data().toString();

    Settings::pushGroup(settingsGroup());
    Settings::remove(name);
    Settings::popGroup();

    if (!updateModel())
        qDebug() << Error::GeneralError;
}

void InterfaceEthernetDialog::setupAddWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;

    QLabel *nameLabel = new QLabel("Имя:", m_addWidget);
    hLayout->addWidget(nameLabel);

    QLineEdit *nameLineEdit = new QLineEdit(m_addWidget);
    nameLineEdit->setObjectName("nameConnection");
    hLayout->addWidget(nameLineEdit);

    mainLayout->addLayout(hLayout);

    hLayout = new QHBoxLayout;
    QLabel *ipLable = new QLabel("IP:", m_addWidget);
    hLayout->addWidget(ipLable);

    for (int i = 0; i < 4; ++i)
    {
        EDoubleSpinBox *ipCell = SPBFunc::New(m_addWidget, QString("ipCell_%1").arg(i), 0, 255, 0);
        hLayout->addWidget(ipCell);
        if (i != 3)
        {
            QLabel *dotLabel = new QLabel(".", m_addWidget);
            hLayout->addWidget(dotLabel);
        }
    }

    mainLayout->addLayout(hLayout);

    hLayout = new QHBoxLayout;
    QLabel *portLabel = new QLabel("Порт:", m_addWidget);
    hLayout->addWidget(portLabel);

    constexpr auto u16min = std::numeric_limits<quint16>::min();
    constexpr auto u16max = std::numeric_limits<quint16>::max();

    EDoubleSpinBox *portCell = SPBFunc::New(m_addWidget, "port", u16min, u16max, 0);
    portCell->setValue(defaultPort());
    hLayout->addWidget(portCell);

    mainLayout->addLayout(hLayout);

    hLayout = new QHBoxLayout;
    QLabel *addressFieldLabel = new QLabel(addressLabel() + ":", m_addWidget);
    hLayout->addWidget(addressFieldLabel);

    EDoubleSpinBox *BSAdressCell = SPBFunc::New(m_addWidget, "BSAdress", u16min, u16max, 0);
    BSAdressCell->setValue(defaultAddress());
    hLayout->addWidget(BSAdressCell);

    mainLayout->addLayout(hLayout);

    hLayout = new QHBoxLayout;
    hLayout->addWidget(PBFunc::New(m_addWidget, "", "Сохранить", this, &InterfaceEthernetDialog::acceptedInterface));
    hLayout->addWidget(PBFunc::New(m_addWidget, "", "Редактировать", this, &InterfaceEthernetDialog::editInterface));

    mainLayout->addLayout(hLayout);

    m_addWidget->setLayout(mainLayout);
}

void InterfaceEthernetDialog::editInterface()
{
    QModelIndex current = m_tableView->currentIndex();
    if (!current.isValid())
    {
        EMessageBox::warning(this, "Не выделена строка для редактирования");
        return;
    }

    QString oldName = current.siblingAtColumn(0).data().toString();
    QString name = LEFunc::data(m_addWidget, "nameConnection");

    QString ipStr = QString("%1.%2.%3.%4")
                        .arg(QString::number(SPBFunc::data<int>(m_addWidget, "ipCell_0")),
                            QString::number(SPBFunc::data<int>(m_addWidget, "ipCell_1")),
                            QString::number(SPBFunc::data<int>(m_addWidget, "ipCell_2")),
                            QString::number(SPBFunc::data<int>(m_addWidget, "ipCell_3")));

    quint16 port = SPBFunc::data<quint16>(m_addWidget, "port");
    quint16 address = SPBFunc::data<quint16>(m_addWidget, "BSAdress");

    if (address == 0)
    {
        EMessageBox::error(this, zeroAddressError());
        return;
    }

    Settings::pushGroup(settingsGroup());

    if (name != oldName)
        Settings::remove(oldName);

    Settings::pushGroup(name);
    Settings::set("ipAddress", ipStr);
    Settings::set("ipPort", port);
    Settings::set(addressSettingsKey(), address);
    Settings::popGroup();
    Settings::popGroup(); // exit from settingsGroup()

    if (!updateModel())
        qDebug() << Error::GeneralError;
}