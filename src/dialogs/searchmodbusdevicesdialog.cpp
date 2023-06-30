#include "searchmodbusdevicesdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QVBoxLayout>

SearchModbusDevicesDialog::SearchModbusDevicesDialog(QWidget *parent) : QDialog(parent)
{
    data.bauds.reserve(widgets.baud.size());
    data.parities.reserve(3);
    data.stopBits.reserve(2);
    setObjectName("rsSearchDevicesDialog");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Поиск устройств");
    setupUI();
}

QGroupBox *SearchModbusDevicesDialog::createComGroupBox()
{
    // Доступные COM-порты
    QStringList comPorts;
    auto portList = QSerialPortInfo::availablePorts();
    comPorts.reserve(portList.size());
    for (const auto &portInfo : portList)
        comPorts << portInfo.portName();

    auto comGroupBox = new QGroupBox("Порт", this);
    auto comGroupBoxLayout = new QVBoxLayout;
    auto portSelect = WDFunc::NewCB2(comGroupBox, "portSelect", comPorts);
    widgets.port = portSelect;
    comGroupBoxLayout->addWidget(portSelect);
    comGroupBox->setLayout(comGroupBoxLayout);
    return comGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createTimeoutGroupBox()
{
    auto timeoutGroupBox = new QGroupBox("Таймаут", this);
    auto timeoutGroupBoxLayout = new QHBoxLayout;
    auto timeoutSelect = WDFunc::NewSPB2(timeoutGroupBox, "timeoutSelect", 100, 5000, 0);
    widgets.timeout = timeoutSelect;
    auto msLabel = WDFunc::NewLBL2(timeoutGroupBox, "мс.");
    timeoutGroupBoxLayout->addWidget(timeoutSelect, Qt::AlignRight);
    timeoutGroupBoxLayout->addWidget(msLabel, Qt::AlignLeft);
    timeoutGroupBox->setLayout(timeoutGroupBoxLayout);
    return timeoutGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createAddressGroupBox()
{
    auto addrGroupBox = new QGroupBox("Адреса", this);
    auto addrGroupBoxLayout = new QVBoxLayout;
    auto firstItemLayout = new QHBoxLayout;
    auto startAddrLabel = WDFunc::NewLBL2(addrGroupBox, "Начало:");
    auto startAddrSelect = WDFunc::NewSPB2(addrGroupBox, "startAddrSelect", 1, 254, 0);
    widgets.startAddr = startAddrSelect;
    firstItemLayout->addWidget(startAddrLabel);
    firstItemLayout->addWidget(startAddrSelect);
    auto secondItemLayout = new QHBoxLayout;
    auto endAddrLabel = WDFunc::NewLBL2(addrGroupBox, "Конец:");
    auto endAddrSelect = WDFunc::NewSPB2(addrGroupBox, "endAddrSelect", 1, 254, 0);
    endAddrSelect->setValue(254);
    widgets.endAddr = endAddrSelect;
    secondItemLayout->addWidget(endAddrLabel);
    secondItemLayout->addWidget(endAddrSelect);
    addrGroupBoxLayout->addLayout(firstItemLayout);
    addrGroupBoxLayout->addLayout(secondItemLayout);
    addrGroupBox->setLayout(addrGroupBoxLayout);
    return addrGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createBaudGroupBox()
{
    auto baudGroupBox = new QGroupBox("Скорости", this);
    auto baudGroupBoxLayout = new QVBoxLayout;
    auto baudGridLayout = new QGridLayout;
    std::size_t index = 0;
    auto baudCheckBox1 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox1", "2400");
    baudGridLayout->addWidget(baudCheckBox1, 0, 0, 1, 1);
    widgets.baud[index++] = baudCheckBox1;
    auto baudCheckBox2 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox2", "4800");
    baudGridLayout->addWidget(baudCheckBox2, 0, 1, 1, 1);
    widgets.baud[index++] = baudCheckBox2;
    auto baudCheckBox3 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox3", "9600");
    baudGridLayout->addWidget(baudCheckBox3, 0, 2, 1, 1);
    widgets.baud[index++] = baudCheckBox3;
    auto baudCheckBox4 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox4", "19200");
    baudGridLayout->addWidget(baudCheckBox4, 1, 0, 1, 1);
    widgets.baud[index++] = baudCheckBox4;
    auto baudCheckBox5 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox5", "38400");
    baudGridLayout->addWidget(baudCheckBox5, 1, 1, 1, 1);
    widgets.baud[index++] = baudCheckBox5;
    auto baudCheckBox6 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox6", "57600");
    baudGridLayout->addWidget(baudCheckBox6, 1, 2, 1, 1);
    widgets.baud[index++] = baudCheckBox6;
    auto baudCheckBox7 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox7", "115200");
    baudGridLayout->addWidget(baudCheckBox7, 2, 0, 1, 1);
    widgets.baud[index] = baudCheckBox7;
    auto buttonsLayout = new QHBoxLayout;
    auto selectAllButton = WDFunc::NewPB(baudGroupBox, "selectAllBaudButton", //
        "Выбрать все", this, [this] {
            for (auto &checkBox : widgets.baud)
                checkBox->setCheckState(Qt::CheckState::Checked);
        });
    buttonsLayout->addWidget(selectAllButton);
    auto clearAllButton = WDFunc::NewPB(baudGroupBox, "clearAllBaudButton", //
        "Очистить все", this, [this] {
            for (auto &checkBox : widgets.baud)
                checkBox->setCheckState(Qt::CheckState::Unchecked);
        });
    buttonsLayout->addWidget(clearAllButton);
    baudGroupBoxLayout->addLayout(baudGridLayout);
    baudGroupBoxLayout->addLayout(buttonsLayout);
    baudGroupBox->setLayout(baudGroupBoxLayout);
    return baudGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createParityGroupBox()
{
    auto parityGroupBox = new QGroupBox("Чётность", this);
    auto parityGroupBoxLayout = new QVBoxLayout;
    auto parityMainLayout = new QHBoxLayout;
    auto parityCheckBox1 = WDFunc::NewChB2(parityGroupBox, "parityCheckBox1", "Нет");
    parityMainLayout->addWidget(parityCheckBox1);
    widgets.parityNone = parityCheckBox1;
    auto parityCheckBox2 = WDFunc::NewChB2(parityGroupBox, "parityCheckBox2", "Нечёт");
    parityMainLayout->addWidget(parityCheckBox2);
    widgets.parityOdd = parityCheckBox2;
    auto parityCheckBox3 = WDFunc::NewChB2(parityGroupBox, "parityCheckBox3", "Чёт");
    parityMainLayout->addWidget(parityCheckBox3);
    widgets.parityEven = parityCheckBox3;
    parityGroupBoxLayout->addLayout(parityMainLayout);
    parityGroupBox->setLayout(parityGroupBoxLayout);
    return parityGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createStopbitsGroupBox()
{
    auto stopbitsGroupBox = new QGroupBox("Стоп-биты", this);
    auto stopbitsGroupBoxLayout = new QVBoxLayout;
    auto stopbitsMainLayout = new QHBoxLayout;
    auto stopbitCheckBox1 = WDFunc::NewChB2(stopbitsGroupBox, "stopbitCheckBox1", "1");
    stopbitsMainLayout->addWidget(stopbitCheckBox1);
    widgets.stopBitOne = stopbitCheckBox1;
    auto stopbitCheckBox2 = WDFunc::NewChB2(stopbitsGroupBox, "stopbitCheckBox2", "2");
    stopbitsMainLayout->addWidget(stopbitCheckBox2);
    widgets.stopBitTwo = stopbitCheckBox2;
    stopbitsGroupBoxLayout->addLayout(stopbitsMainLayout);
    stopbitsGroupBox->setLayout(stopbitsGroupBoxLayout);
    return stopbitsGroupBox;
}

QHBoxLayout *SearchModbusDevicesDialog::createControlLayout()
{
    auto controlLayout = new QHBoxLayout;
    auto runSearchButton = WDFunc::NewPB(this, "runSearchButton", //
        "Поиск", this, [this] { runSearch(); });
    auto cancelButton = WDFunc::NewPB(this, "cancelButton", //
        "Отмена", this, [this] { close(); });
    controlLayout->addWidget(runSearchButton);
    controlLayout->addWidget(cancelButton);
    return controlLayout;
}

void SearchModbusDevicesDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    auto firstRow = new QHBoxLayout;
    firstRow->addWidget(createComGroupBox());
    firstRow->addWidget(createTimeoutGroupBox());
    auto secondRow = new QVBoxLayout;
    secondRow->addWidget(createAddressGroupBox());
    secondRow->addWidget(createBaudGroupBox());
    secondRow->addWidget(createParityGroupBox());
    secondRow->addWidget(createStopbitsGroupBox());
    mainLayout->addLayout(firstRow);
    mainLayout->addLayout(secondRow);
    mainLayout->addLayout(createControlLayout());
    setLayout(mainLayout);
    adjustSize();
}

void SearchModbusDevicesDialog::getData()
{
    data.bauds.clear();
    data.parities.clear();
    data.stopBits.clear();

    data.port = widgets.port->currentText();
    data.timeout = static_cast<int>(widgets.timeout->value());
    data.startAddr = static_cast<int>(widgets.startAddr->value());
    data.endAddr = static_cast<int>(widgets.endAddr->value());
    for (auto &checkBox : widgets.baud)
        if (checkBox->isChecked())
            data.bauds.push_back(static_cast<qint32>(checkBox->text().toInt()));

    if (widgets.parityNone->isChecked())
        data.parities.push_back(QSerialPort::Parity::NoParity);
    if (widgets.parityOdd->isChecked())
        data.parities.push_back(QSerialPort::Parity::OddParity);
    if (widgets.parityEven->isChecked())
        data.parities.push_back(QSerialPort::Parity::EvenParity);
    if (widgets.stopBitOne->isChecked())
        data.stopBits.push_back(QSerialPort::StopBits::OneStop);
    if (widgets.stopBitTwo->isChecked())
        data.stopBits.push_back(QSerialPort::StopBits::TwoStop);
}

bool SearchModbusDevicesDialog::validate()
{
    // Проверка диапазона адресов
    if (data.startAddr > data.endAddr)
    {
        EMessageBox::warning(this, "Начальный адрес должен быть меньше или равен конечному");
        return false;
    }
    // Проверка выбранных скоростей
    if (data.bauds.empty())
    {
        EMessageBox::warning(this, "Необходимо выбрать хотя бы одну скорость");
        return false;
    }
    // Проверка выбранных параметров чётности
    if (data.parities.empty())
    {
        EMessageBox::warning(this, "Необходимо выбрать хотя бы один параметр чётности");
        return false;
    }
    // Проверка выбранных параметров стоповых битов
    if (data.stopBits.empty())
    {
        EMessageBox::warning(this, "Необходимо выбрать хотя бы один параметр стоп-битов");
        return false;
    }
    return true;
}

void SearchModbusDevicesDialog::runSearch()
{
    getData();
    if (validate())
    {
        auto proccessDlg = new SearchProccessDialog(data, this);
        proccessDlg->show();
        proccessDlg->search();
    }
}
