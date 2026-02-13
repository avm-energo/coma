#include "dialogs/interfaceserialdialog.h"

#include <dialogs/searchmodbusdevicesdialog.h>
#include <avm-gen/error.h>
#include <avm-gen/settings.h>
#include <avm-gen/stdfunc.h>
#include <interfaces/types/serial_settings.h>
#include <avm-widgets/cbfunc.h>
#include <avm-widgets/emessagebox.h>
#include <avm-widgets/lblfunc.h>
#include <avm-widgets/lefunc.h>
#include <avm-widgets/pbfunc.h>
#include <avm-widgets/spbfunc.h>
#include <avm-widgets/tvfunc.h>

#include <QSerialPortInfo>
#include <QStandardItemModel>
#include <QVBoxLayout>

InterfaceSerialDialog::InterfaceSerialDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
    setWindowTitle("RS485 соединения");
}

InterfaceSerialDialog::~InterfaceSerialDialog() noexcept { }

void InterfaceSerialDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    m_tableView = TVFunc::New(this, "", nullptr);
    lyout->addWidget(m_tableView);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto firstRow = new QHBoxLayout;
    auto addButton = PBFunc::New(this, "newrspb", "Добавить", this, &InterfaceSerialDialog::addInterface);
    auto removeButton = PBFunc::New(this, "", "Удалить", this,
        [this]
        {
            auto name = m_tableView->currentIndex().siblingAtColumn(0).data().toString();
            Settings::pushGroup("RS485");
            Settings::remove(name);
            Settings::popGroup();
            updateModel();
        });
    firstRow->addWidget(addButton);
    firstRow->addWidget(removeButton);
    lyout->addLayout(firstRow);

    auto secondRow = new QHBoxLayout;
    auto editButton = PBFunc::New(this, "", "Редактировать", this,
        [this]
        {
            auto index = m_tableView->currentIndex();
            editConnection(index);
            updateModel();
        });
    auto searchButton = PBFunc::New(this, "", "Поиск устройств", this,
        [this]
        {
            auto searchDialog = new SearchModbusDevicesDialog(this);
            searchDialog->exec();
        });
    secondRow->addWidget(editButton);
    secondRow->addWidget(searchButton);
    lyout->addLayout(secondRow);

    setLayout(lyout);
    connect(m_tableView, &QTableView::doubleClicked, this, &InterfaceSerialDialog::setInterface);
    AbstractInterfaceDialog::setupUI();
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
    if (parityStr == "Нет")
        settings->set("parity", QSerialPort::NoParity);
    else if (parityStr == "Чет")
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

    // Create dialog
    QStringList ports;
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : portlist)
        ports << info.portName();
    auto dialog = new QDialog(this);
    dialog->setObjectName("rsCreateDialog");
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    auto layout = new QGridLayout;
    int count = 0;
    layout->addWidget(LBLFunc::New(dialog, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    auto namele = LEFunc::New(dialog, "namele", name);
    layout->addWidget(namele, count++, 1, 1, 1);
    layout->addWidget(LBLFunc::New(dialog, "Порт:"), count, 0, 1, 1, Qt::AlignLeft);
    auto portcb = CBFunc::New(dialog, "portcb", ports);
    auto portIndex = ports.indexOf(port);                    // найти индекс сохранённого порта
    portcb->setCurrentIndex(portIndex >= 0 ? portIndex : 0); // проверить и выставить порт по умолчанию
    layout->addWidget(portcb, count++, 1, 1, 1);
    QStringList sl { "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    layout->addWidget(LBLFunc::New(dialog, "Скорость:"), count, 0, 1, 1, Qt::AlignLeft);
    auto speedcb = CBFunc::New(dialog, "speedcb", sl);
    speedcb->setCurrentIndex(sl.indexOf(speed));
    layout->addWidget(speedcb, count++, 1, 1, 1);
    sl = QStringList({ "Нет", "Нечет", "Чет" });
    layout->addWidget(LBLFunc::New(dialog, "Чётность:"), count, 0, 1, 1, Qt::AlignLeft);
    auto paritycb = CBFunc::New(dialog, "paritycb", sl);
    paritycb->setCurrentIndex(sl.indexOf(parity));
    layout->addWidget(paritycb, count++, 1, 1, 1);
    layout->addWidget(LBLFunc::New(dialog, "Стоп бит:"), count, 0, 1, 1, Qt::AlignLeft);
    sl = QStringList({ "1", "2" });
    auto stopbitcb = CBFunc::New(dialog, "stopbitcb", sl);
    stopbitcb->setCurrentIndex(sl.indexOf(stopbit));
    layout->addWidget(stopbitcb, count++, 1, 1, 1);
    layout->addWidget(LBLFunc::New(dialog, "Адрес:"), count, 0, 1, 1, Qt::AlignLeft);
    auto addressspb = SPBFunc::New(dialog, "addressspb", 0, 255, 0);
    addressspb->setValue(address);
    layout->addWidget(addressspb, count++, 1, 1, 1);

    // Logic of working
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(PBFunc::New(dialog, "acceptpb", "Сохранить", dialog,
        [&]
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
            dialog->close();
        }));
    hlyout->addWidget(PBFunc::New(dialog, "cancelpb", "Отмена", dialog, [dialog] { dialog->close(); }));
    layout->addLayout(hlyout, count, 0, 1, 2, Qt::AlignCenter);
    dialog->setLayout(layout);
    dialog->adjustSize();
    dialog->exec();
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
    for (const QSerialPortInfo &info : portlist)
        ports << info.portName();
    auto dialog = new QDialog(this);
    dialog->setObjectName("rsCreateDialog");
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    auto layout = new QGridLayout;
    int count = 0;
    layout->addWidget(LBLFunc::New(dialog, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(LEFunc::New(dialog, "namele"), count++, 1, 1, 1);
    layout->addWidget(LBLFunc::New(dialog, "Порт:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(CBFunc::New(dialog, "portcb", ports), count++, 1, 1, 1);
    QStringList sl { "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    layout->addWidget(LBLFunc::New(dialog, "Скорость:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(CBFunc::New(dialog, "speedcb", sl), count++, 1, 1, 1);
    sl = QStringList({ "Нет", "Нечет", "Чет" });
    layout->addWidget(LBLFunc::New(dialog, "Чётность:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(CBFunc::New(dialog, "paritycb", sl), count++, 1, 1, 1);
    layout->addWidget(LBLFunc::New(dialog, "Стоп бит:"), count, 0, 1, 1, Qt::AlignLeft);
    sl = QStringList({ "1", "2" });
    layout->addWidget(CBFunc::New(dialog, "stopbitcb", sl), count++, 1, 1, 1);
    layout->addWidget(LBLFunc::New(dialog, "Адрес:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(SPBFunc::New(dialog, "addressspb", 0, 255, 0), count++, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(PBFunc::New(dialog, "acceptpb", "Сохранить", this, &InterfaceSerialDialog::acceptedInterface));
    hlyout->addWidget(PBFunc::New(dialog, "cancelpb", "Отмена", dialog, [dialog] { dialog->close(); }));
    layout->addLayout(hlyout, count, 0, 1, 2, Qt::AlignCenter);
    dialog->setLayout(layout);
    dialog->adjustSize();
    dialog->exec();
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
    m_tableView->resizeColumnsToContents();
    return true;
}

void InterfaceSerialDialog::acceptedInterface()
{
    auto dialog = this->findChild<QDialog *>("rsCreateDialog");
    if (dialog == nullptr)
        return;
    QString name = LEFunc::data(dialog, "namele");
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
    Settings::set("serialPort", CBFunc::data(dialog, "portcb"));
    Settings::set("serialSpeed", CBFunc::data(dialog, "speedcb"));
    Settings::set("serialParity", CBFunc::data(dialog, "paritycb"));
    Settings::set("serialStop", CBFunc::data(dialog, "stopbitcb"));
    if (SPBFunc::data(dialog, "addressspb", spbdata))
        Settings::set("modbusAddress", spbdata);
    Settings::popGroup();
    Settings::popGroup(); // exit from RS-485
    if (!updateModel())
        qDebug() << Error::GeneralError;
    dialog->close();
}
