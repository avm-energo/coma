#include "dialogs/connDialogs/serialDialog/interfaceserialdialog.h"

#include "const.h"
#include "interfaces/utils/utils.h"
#include <dialogs/connDialogs/serialDialog/searchmodbusdevicesdialog.h>
#include <interfaces/types/serial_settings.h>
#include <libavm-gen/error.h>
#include <libavm-gen/settings.h>
#include <libavm-gen/stdfunc.h>
#include <libavm-widgets/cbfunc.h>
#include <libavm-widgets/emessagebox.h>
#include <libavm-widgets/lblfunc.h>
#include <libavm-widgets/lefunc.h>
#include <libavm-widgets/pbfunc.h>
#include <libavm-widgets/spbfunc.h>
#include <libavm-widgets/tvfunc.h>

#include <QHeaderView>
#include <QSerialPortInfo>
#include <QStandardItemModel>
#include <QVBoxLayout>

InterfaceSerialDialog::InterfaceSerialDialog(QWidget *parent) : AbstractInterfaceDialog(parent) { }

InterfaceSerialDialog::~InterfaceSerialDialog() noexcept { }

bool InterfaceSerialDialog::isBusy() const
{
    return m_busy;
}

void InterfaceSerialDialog::setBusy(bool busy)
{
    m_busy = busy;
}

void InterfaceSerialDialog::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *buttonLayout = new QVBoxLayout;

    QVBoxLayout *formLayout = new QVBoxLayout(m_formArea);
    formLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *addButton = PBFunc::New(this, "", "Добавить", this, &InterfaceSerialDialog::addInterface);
    QPushButton *removeButton = PBFunc::New(this, "", "Удалить", this, &InterfaceSerialDialog::deleteInterface);
    QPushButton *editButton = PBFunc::New(this, "", "Редактировать", this, &InterfaceSerialDialog::editInterface);
    QPushButton *cancelButton = PBFunc::New(this, "", "Назад", this, &QDialog::close);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(m_formArea, 1);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout, 3);

    m_tableView = TVFunc::New(this, "", nullptr);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_tableView, 7);

    mainLayout->addWidget(new SearchModbusDevicesDialog(this, this), 8);

    setLayout(mainLayout);

    connect(m_tableView, &QTableView::doubleClicked, this, &InterfaceSerialDialog::setInterface);
    // connect del
}

void InterfaceSerialDialog::setInterface(QModelIndex index)
{
    auto model = index.model();
    int row = index.row();
    QString name = model->data(model->index(row, 0)).toString();
    SerialSettings *settings = new SerialSettings;

    settings->set("name", model->data(model->index(row, 1)).toString());
    settings->set("baud", model->data(model->index(row, 2)).toUInt());

    auto parityStr = model->data(model->index(row, 3)).toString();
    if (parityStr == ParityRS485::noParity)
        settings->set("parity", QSerialPort::NoParity);
    else if (parityStr == ParityRS485::evenParity)
        settings->set("parity", QSerialPort::EvenParity);
    else
        settings->set("parity", QSerialPort::OddParity);

    auto stopStr = model->data(model->index(row, 4)).toString();

    settings->set("stop", (stopStr == "1") ? QSerialPort::OneStop : QSerialPort::TwoStop);
    settings->set("address", model->data(model->index(row, 5)).toUInt());
    settings->set("timeout", Settings::get("modbusTimeout", 3000));
    settings->set("reconnectInterval", Settings::get("modbusReconnect", 1000));
    apply(settings);

    if (!settings->isValid())
        return;

    ConnectionSettings st { name, settings };
    emit accepted(st);
}

void InterfaceSerialDialog::editConnection(QModelIndex index)
{
    if (!index.isValid())
        return;

    // Getting data from model
    auto model = index.model();
    int row = index.row();
    auto name = model->data(model->index(row, 0)).toString();
    auto port = model->data(model->index(row, 1)).toString();
    auto speed = model->data(model->index(row, 2)).toString();
    auto parity = model->data(model->index(row, 3)).toString();
    auto stopbit = model->data(model->index(row, 4)).toString();
    auto address = model->data(model->index(row, 5)).toInt();

    QStringList ports;
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : std::as_const(portlist))
        ports << info.portName();

    auto *form = new QWidget(m_formArea);
    auto *layout = new QVBoxLayout(form);

    layout->addWidget(LBLFunc::New(form, "Имя:"));
    auto namele = LEFunc::New(form, "namele", name);
    layout->addWidget(namele);

    layout->addWidget(LBLFunc::New(form, "Порт:"));
    auto portcb = CBFunc::New(form, "portcb", ports);
    auto portIndex = ports.indexOf(port);                    // найти индекс сохранённого порта
    portcb->setCurrentIndex(portIndex >= 0 ? portIndex : 0); // проверить и выставить порт по умолчанию
    layout->addWidget(portcb);

    QStringList sl = SerialUtils::allBaudValuesRS485();
    layout->addWidget(LBLFunc::New(form, "Скорость:"));
    auto speedcb = CBFunc::New(form, "speedcb", sl);
    speedcb->setCurrentIndex(sl.indexOf(speed));
    layout->addWidget(speedcb);

    sl = QStringList({ ParityRS485::noParity, ParityRS485::oddParity, ParityRS485::evenParity });
    layout->addWidget(LBLFunc::New(form, "Четность:"));
    auto paritycb = CBFunc::New(form, "paritycb", sl);
    paritycb->setCurrentIndex(sl.indexOf(parity));
    layout->addWidget(paritycb);

    sl = QStringList({ "1", "2" });
    layout->addWidget(LBLFunc::New(form, "Стоп бит:"));
    auto stopbitcb = CBFunc::New(form, "stopbitcb", sl);
    stopbitcb->setCurrentIndex(sl.indexOf(stopbit));
    layout->addWidget(stopbitcb);

    layout->addWidget(LBLFunc::New(form, "Адрес:"));
    auto addressspb = SPBFunc::New(form, "addressspb", 0, 255, 0);
    addressspb->setValue(address);
    layout->addWidget(addressspb);

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(PBFunc::New(form, "acceptpb", "Сохранить", form,
        [this, name, namele, portcb, speedcb, paritycb, stopbitcb, addressspb]
        {
            auto newName = namele->text();
            Settings::pushGroup("RS485");

            // Новое имя не совпадает со старым, но уже имеется в настройках
            if (newName != name && Settings::groupExist(newName))
            {
                Settings::popGroup();
                EMessageBox::error(this, "Такое имя уже имеется");
                return;
            }
            Settings::remove(name);

            Settings::pushGroup(newName);
            Settings::set("serialPort", portcb->currentText());
            Settings::set("serialSpeed", speedcb->currentText());
            Settings::set("serialParity", paritycb->currentText());
            Settings::set("serialStop", stopbitcb->currentText());
            Settings::set("modbusAddress", static_cast<int>(addressspb->value()));
            Settings::popGroup();
            Settings::popGroup(); // exit from RS-485

            if (!updateModel())
                qDebug() << Error::GeneralError;
            clearForm();
        }));
    hlyout->addWidget(PBFunc::New(form, "cancelpb", "Отмена", this, [this] { clearForm(); }));
    layout->addLayout(hlyout);

    showForm(form);
}

void InterfaceSerialDialog::addInterface()
{
    if (checkSize())
    {
        EMessageBox::warning(this, "Превышен лимит соединений!");
        return;
    }

    QStringList ports;
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : std::as_const(portlist))
        ports << info.portName();

    auto *form = new QWidget(m_formArea);
    auto *layout = new QVBoxLayout(form);

    layout->addWidget(LBLFunc::New(form, "Имя:"));
    layout->addWidget(LEFunc::New(form, "namele"));
    layout->addWidget(LBLFunc::New(form, "Порт:"));
    layout->addWidget(CBFunc::New(form, "portcb", ports));

    QStringList sl = SerialUtils::allBaudValuesRS485();
    layout->addWidget(LBLFunc::New(form, "Скорость:"));
    layout->addWidget(CBFunc::New(form, "speedcb", sl));

    sl = QStringList({ ParityRS485::noParity, ParityRS485::oddParity, ParityRS485::evenParity });
    layout->addWidget(LBLFunc::New(form, "Четность:"));
    layout->addWidget(CBFunc::New(form, "paritycb", sl));
    layout->addWidget(LBLFunc::New(form, "Стоп бит:"));

    sl = QStringList({ SerialUtils::stopBitsToString(QSerialPort::StopBits::OneStop),
        SerialUtils::stopBitsToString(QSerialPort::StopBits::TwoStop) });
    layout->addWidget(CBFunc::New(form, "stopbitcb", sl));
    layout->addWidget(LBLFunc::New(form, "Адрес:"));
    layout->addWidget(SPBFunc::New(form, "addressspb", 0, 255, 0));

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(PBFunc::New(form, "acceptpb", "Сохранить", this, &InterfaceSerialDialog::acceptedInterface));
    hlyout->addWidget(PBFunc::New(form, "cancelpb", "Отмена", this, [this] { clearForm(); }));
    layout->addLayout(hlyout);

    showForm(form);
}

bool InterfaceSerialDialog::updateModel()
{
    QStringList headers { "Имя", "Порт", "Скорость", "Четность", "Стоп бит", "Адрес" };
    auto tableViewModel = static_cast<QStandardItemModel *>(m_tableView->model());

    if (tableViewModel == nullptr)
        tableViewModel = new QStandardItemModel(this);
    else
        tableViewModel->clear();

    tableViewModel->setHorizontalHeaderLabels(headers);

    Settings::pushGroup("RS485");
    auto rslist = Settings::groups();

    for (const auto &item : std::as_const(rslist))
    {
        Settings::pushGroup(item);
        QList<QStandardItem *> items {
            new QStandardItem(item),                                                 //
            new QStandardItem(QString(Settings::get("serialPort", "/dev/ttyUSB0"))), //
            new QStandardItem(QString(Settings::get("serialSpeed", 115200))),        //
            new QStandardItem(QString(Settings::get("serialParity", "Нет"))),        //
            new QStandardItem(QString(Settings::get("serialStop", 1))),              //
            new QStandardItem(QString(Settings::get("modbusAddress", 1))),           //
        };
        Settings::popGroup();
        tableViewModel->appendRow(items);
        // m_settings.endGroup();
    }
    Settings::popGroup();
    m_tableView->setModel(tableViewModel);

    return true;
}

void InterfaceSerialDialog::acceptedInterface()
{
    QString name = LEFunc::data(m_formArea, "namele");

    // check if there's such name in registry
    Settings::pushGroup("RS485");
    if (Settings::groupExist(name))
    {
        Settings::popGroup();
        EMessageBox::error(this, "Такое имя уже имеется");
        return;
    }

    int spbdata;
    Settings::pushGroup(name);
    Settings::set("serialPort", CBFunc::data(m_formArea, "portcb"));
    Settings::set("serialSpeed", CBFunc::data(m_formArea, "speedcb"));
    Settings::set("serialParity", CBFunc::data(m_formArea, "paritycb"));
    Settings::set("serialStop", CBFunc::data(m_formArea, "stopbitcb"));

    if (SPBFunc::data(m_formArea, "addressspb", spbdata))
        Settings::set("modbusAddress", spbdata);

    Settings::popGroup();
    Settings::popGroup(); // exit from RS-485

    if (!updateModel())
        qDebug() << Error::GeneralError;

    clearForm();
}

void InterfaceSerialDialog::showForm(QWidget *form)
{
    clearForm();
    auto *layout = static_cast<QVBoxLayout *>(m_formArea->layout());
    layout->addStretch();
    layout->addWidget(form);
    layout->addStretch();
}

void InterfaceSerialDialog::clearForm()
{
    QLayoutItem *item;
    while ((item = m_formArea->layout()->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }
}

QStringList InterfaceSerialDialog::getConnectionNames() const
{
    QStringList names;
    Settings::pushGroup("RS485");
    auto rslist = Settings::groups();

    for (const auto &item : std::as_const(rslist))
    {
        names << item;
    }
    Settings::popGroup();

    return names;
}

void InterfaceSerialDialog::removeConnection(const QString &name)
{
    Settings::pushGroup("RS485");
    Settings::remove(name);
    Settings::popGroup();
    updateModel();
}

void InterfaceSerialDialog::addConnectionFromSearch(const QMap<QString, QVariant> &deviceData)
{
    if (checkSize())
        return;

    QString name = deviceData.value(SerialKeysRS485::name).toString();

    Settings::pushGroup("RS485");
    if (Settings::groupExist(name))
    {
        Settings::popGroup();
        return;
    }

    Settings::pushGroup(name);
    Settings::set("serialPort", deviceData.value(SerialKeysRS485::port).toString());
    Settings::set("serialSpeed", deviceData.value(SerialKeysRS485::baud).toString());
    Settings::set("serialParity", deviceData.value(SerialKeysRS485::parity).toString());
    Settings::set("serialStop", deviceData.value(SerialKeysRS485::stopBits).toString());
    Settings::set("modbusAddress", deviceData.value(SerialKeysRS485::address).toInt());
    Settings::popGroup(); // name
    Settings::popGroup(); // RS485

    updateModel();
}

void InterfaceSerialDialog::deleteInterface()
{
    QString name = m_tableView->currentIndex().siblingAtColumn(0).data().toString();

    Settings::pushGroup("RS485");
    Settings::remove(name);
    Settings::popGroup();

    updateModel();
}

void InterfaceSerialDialog::editInterface()
{
    auto index = m_tableView->currentIndex();
    editConnection(index);
}
