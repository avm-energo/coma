#include "interfaceserialdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"
#include "searchmodbusdevicesdialog.h"

#include <QSerialPortInfo>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <interfaces/types/settingstypes.h>

InterfaceSerialDialog::InterfaceSerialDialog(QWidget *parent) : AbstractInterfaceDialog(parent)
{
    m_settings.native().beginGroup("RS485");
    setWindowTitle("RS485 соединения");
}

InterfaceSerialDialog::~InterfaceSerialDialog() noexcept
{
    m_settings.native().endGroup();
}

void InterfaceSerialDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    m_tableView = WDFunc::NewQTV(this, "", nullptr);
    lyout->addWidget(m_tableView);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto firstRow = new QHBoxLayout;
    auto addButton = WDFunc::NewPB(this, "newrspb", "Добавить", this, &InterfaceSerialDialog::addInterface);
    auto removeButton = WDFunc::NewPB(this, "", "Удалить", this, [this] {
        auto name = m_tableView->currentIndex().siblingAtColumn(0).data().toString();
        m_settings.remove(name);
        updateModel();
    });
    firstRow->addWidget(addButton);
    firstRow->addWidget(removeButton);
    lyout->addLayout(firstRow);

    auto secondRow = new QHBoxLayout;
    auto editButton = WDFunc::NewPB(this, "", "Редактировать", this, [this] {
        auto index = m_tableView->currentIndex();
        editConnection(index);
        updateModel();
    });
    auto searchButton = WDFunc::NewPB(this, "", "Поиск устройств", this, [this] {
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
    SerialPortSettings portSettings;
    portSettings.name = model->data(model->index(row, 1)).toString();
    portSettings.baud = model->data(model->index(row, 2)).toUInt();
    auto parityStr = model->data(model->index(row, 3)).toString();
    if (parityStr == "Нет")
        portSettings.parity = QSerialPort::NoParity;
    else if (parityStr == "Чет")
        portSettings.parity = QSerialPort::EvenParity;
    else
        portSettings.parity = QSerialPort::OddParity;
    auto stopStr = model->data(model->index(row, 4)).toString();
    portSettings.stop = stopStr == "1" ? QSerialPort::OneStop : QSerialPort::TwoStop;
    portSettings.address = model->data(model->index(row, 5)).toUInt();
    if (!portSettings.isValid())
        return;
    ConnectStruct st { name, portSettings };
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
    layout->addWidget(WDFunc::NewLBL2(dialog, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    auto namele = WDFunc::NewLE2(dialog, "namele", name);
    layout->addWidget(namele, count++, 1, 1, 1);
    layout->addWidget(WDFunc::NewLBL2(dialog, "Порт:"), count, 0, 1, 1, Qt::AlignLeft);
    auto portcb = WDFunc::NewCB2(dialog, "portcb", ports);
    auto portIndex = ports.indexOf(port); // найти индекс сохранённого порта
    portcb->setCurrentIndex(portIndex >= 0 ? portIndex : 0); // проверить и выставить порт по умолчанию
    layout->addWidget(portcb, count++, 1, 1, 1);
    QStringList sl { "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    layout->addWidget(WDFunc::NewLBL2(dialog, "Скорость:"), count, 0, 1, 1, Qt::AlignLeft);
    auto speedcb = WDFunc::NewCB2(dialog, "speedcb", sl);
    speedcb->setCurrentIndex(sl.indexOf(speed));
    layout->addWidget(speedcb, count++, 1, 1, 1);
    sl = QStringList({ "Нет", "Нечет", "Чет" });
    layout->addWidget(WDFunc::NewLBL2(dialog, "Чётность:"), count, 0, 1, 1, Qt::AlignLeft);
    auto paritycb = WDFunc::NewCB2(dialog, "paritycb", sl);
    paritycb->setCurrentIndex(sl.indexOf(parity));
    layout->addWidget(paritycb, count++, 1, 1, 1);
    layout->addWidget(WDFunc::NewLBL2(dialog, "Стоп бит:"), count, 0, 1, 1, Qt::AlignLeft);
    sl = QStringList({ "1", "2" });
    auto stopbitcb = WDFunc::NewCB2(dialog, "stopbitcb", sl);
    stopbitcb->setCurrentIndex(sl.indexOf(stopbit));
    layout->addWidget(stopbitcb, count++, 1, 1, 1);
    layout->addWidget(WDFunc::NewLBL2(dialog, "Адрес:"), count, 0, 1, 1, Qt::AlignLeft);
    auto addressspb = WDFunc::NewSPB2(dialog, "addressspb", 0, 255, 0);
    addressspb->setValue(address);
    layout->addWidget(addressspb, count++, 1, 1, 1);

    // Logic of working
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(dialog, "acceptpb", "Сохранить", dialog, [=] {
        auto newName = namele->text();
        // Новое имя не совпадает со старым, но уже имеется в настройках
        if (newName != name && m_settings.isExist(newName))
        {
            EMessageBox::error(this, "Такое имя уже имеется");
            return;
        }
        m_settings.remove(name);

        Settings::ScopedSettingsGroup _ { m_settings, newName };
        m_settings.set<Settings::SerialPort>(portcb->currentText());
        m_settings.set<Settings::SerialSpeed>(speedcb->currentText());
        m_settings.set<Settings::SerialParity>(paritycb->currentText());
        m_settings.set<Settings::SerialStop>(stopbitcb->currentText());
        m_settings.set<Settings::ModbusAddress>(static_cast<int>(addressspb->value()));
        if (!updateModel())
            qCritical() << Error::GeneralError;
        dialog->close();
    }));
    hlyout->addWidget(WDFunc::NewPB(dialog, "cancelpb", "Отмена", dialog, [dialog] { dialog->close(); }));
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
    layout->addWidget(WDFunc::NewLBL2(dialog, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(WDFunc::NewLE2(dialog, "namele"), count++, 1, 1, 1);
    layout->addWidget(WDFunc::NewLBL2(dialog, "Порт:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(WDFunc::NewCB2(dialog, "portcb", ports), count++, 1, 1, 1);
    QStringList sl { "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    layout->addWidget(WDFunc::NewLBL2(dialog, "Скорость:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(WDFunc::NewCB2(dialog, "speedcb", sl), count++, 1, 1, 1);
    sl = QStringList({ "Нет", "Нечет", "Чет" });
    layout->addWidget(WDFunc::NewLBL2(dialog, "Чётность:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(WDFunc::NewCB2(dialog, "paritycb", sl), count++, 1, 1, 1);
    layout->addWidget(WDFunc::NewLBL2(dialog, "Стоп бит:"), count, 0, 1, 1, Qt::AlignLeft);
    sl = QStringList({ "1", "2" });
    layout->addWidget(WDFunc::NewCB2(dialog, "stopbitcb", sl), count++, 1, 1, 1);
    layout->addWidget(WDFunc::NewLBL2(dialog, "Адрес:"), count, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(WDFunc::NewSPB2(dialog, "addressspb", 0, 255, 0), count++, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(dialog, "acceptpb", "Сохранить", this, &InterfaceSerialDialog::acceptedInterface));
    hlyout->addWidget(WDFunc::NewPB(dialog, "cancelpb", "Отмена", dialog, [dialog] { dialog->close(); }));
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

    auto rslist = m_settings.native().childGroups();
    for (const auto &item : qAsConst(rslist))
    {
        Settings::ScopedSettingsGroup _ { m_settings, item };
        // m_settings.beginGroup(item);
        QList<QStandardItem *> items {
            new QStandardItem(item),                                               //
            new QStandardItem(QString(m_settings.get<Settings::SerialPort>())),    //
            new QStandardItem(QString(m_settings.get<Settings::SerialSpeed>())),   //
            new QStandardItem(QString(m_settings.get<Settings::SerialParity>())),  //
            new QStandardItem(QString(m_settings.get<Settings::SerialStop>())),    //
            new QStandardItem(QString(m_settings.get<Settings::ModbusAddress>())), //
        };
        tableViewModel->appendRow(items);
        // m_settings.endGroup();
    }
    m_tableView->setModel(tableViewModel);
    m_tableView->resizeColumnsToContents();
    return true;
}

void InterfaceSerialDialog::acceptedInterface()
{
    auto dialog = this->findChild<QDialog *>("rsCreateDialog");
    if (dialog == nullptr)
        return;
    QString name = WDFunc::LEData(dialog, "namele");
    // check if there's such name in registry
    if (m_settings.isExist(name))
    {
        EMessageBox::error(this, "Такое имя уже имеется");
        return;
    }
    {
        int spbdata;
        Settings::ScopedSettingsGroup _ { m_settings, name };
        m_settings.set<Settings::SerialPort>(WDFunc::CBData(dialog, "portcb"));
        m_settings.set<Settings::SerialSpeed>(WDFunc::CBData(dialog, "speedcb"));
        m_settings.set<Settings::SerialParity>(WDFunc::CBData(dialog, "paritycb"));
        m_settings.set<Settings::SerialStop>(WDFunc::CBData(dialog, "stopbitcb"));
        if (WDFunc::SPBData(dialog, "addressspb", spbdata))
            m_settings.set<Settings::ModbusAddress>(spbdata);
    }
    if (!updateModel())
        qCritical() << Error::GeneralError;
    dialog->close();
}
