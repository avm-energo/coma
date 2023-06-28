#include "searchmodbusdevicesdialog.h"

#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QVBoxLayout>
#include <variant>
#include <vector>

SearchModbusDevicesDialog::SearchModbusDevicesDialog(QWidget *parent) : QDialog(parent)
{
    setObjectName("rsSearchDevicesDialog");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Поиск устройств");
    setupUI();
}

void SearchModbusDevicesDialog::selectAllBaudCheckBoxes()
{
    for (auto &checkBox : widgets.baud)
        checkBox->setCheckState(Qt::CheckState::Checked);
}

void SearchModbusDevicesDialog::clearAllBaudCheckBoxes()
{
    for (auto &checkBox : widgets.baud)
        checkBox->setCheckState(Qt::CheckState::Unchecked);
}

QGroupBox *SearchModbusDevicesDialog::createComGroupBox()
{
    QStringList comPorts;
    auto portList = QSerialPortInfo::availablePorts();
    comPorts.reserve(portList.size());
    for (const auto &portInfo : portList)
        comPorts << portInfo.portName();

    // COM group box
    auto comGroupBox = new QGroupBox("COM для поиска", this);
    auto comGroupBoxLayout = new QVBoxLayout;
    auto comSelect = WDFunc::NewCB2(comGroupBox, "comSelect", comPorts);
    widgets.com = comSelect;
    comGroupBoxLayout->addWidget(comSelect);
    comGroupBox->setLayout(comGroupBoxLayout);
    return comGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createTimeoutGroupBox()
{
    // Timeout group box
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
    // Addresses group box
    auto addrGroupBox = new QGroupBox("Адреса", this);
    auto addrGroupBoxLayout = new QVBoxLayout;
    auto firstItemLayout = new QHBoxLayout;
    auto startAddrLabel = WDFunc::NewLBL2(addrGroupBox, "Начало:");
    auto startAddrSelect = WDFunc::NewSPB2(addrGroupBox, "startAddrSelect", 0, 255, 0);
    widgets.startAddr = startAddrSelect;
    firstItemLayout->addWidget(startAddrLabel, Qt::AlignRight);
    firstItemLayout->addWidget(startAddrSelect, Qt::AlignLeft);
    auto secondItemLayout = new QHBoxLayout;
    auto endAddrLabel = WDFunc::NewLBL2(addrGroupBox, "Конец:");
    auto endAddrSelect = WDFunc::NewSPB2(addrGroupBox, "endAddrSelect", 0, 255, 0);
    widgets.endAddr = endAddrSelect;
    secondItemLayout->addWidget(endAddrLabel, Qt::AlignRight);
    secondItemLayout->addWidget(endAddrSelect, Qt::AlignLeft);
    addrGroupBoxLayout->addLayout(firstItemLayout);
    addrGroupBoxLayout->addLayout(secondItemLayout);
    addrGroupBox->setLayout(addrGroupBoxLayout);
    return addrGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createBaudGroupBox()
{
    // Baud rate group box
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
    widgets.baud[index++] = baudCheckBox7;
    auto buttonsLayout = new QHBoxLayout;
    auto selectAllButton = WDFunc::NewPB(baudGroupBox, "selectAllButton", "Выбрать все", //
        this, &SearchModbusDevicesDialog::selectAllBaudCheckBoxes);
    buttonsLayout->addWidget(selectAllButton);
    auto clearAllButton = WDFunc::NewPB(baudGroupBox, "clearAllButton", "Очистить все", //
        this, &SearchModbusDevicesDialog::clearAllBaudCheckBoxes);
    buttonsLayout->addWidget(clearAllButton);
    baudGroupBoxLayout->addLayout(baudGridLayout);
    baudGroupBoxLayout->addLayout(buttonsLayout);
    baudGroupBox->setLayout(baudGroupBoxLayout);
    return baudGroupBox;
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

    ;
    ;
    ;

    mainLayout->addLayout(firstRow);
    mainLayout->addLayout(secondRow);
    ;
    setLayout(mainLayout);
    adjustSize();
}
