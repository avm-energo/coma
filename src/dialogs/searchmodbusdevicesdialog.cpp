#include "searchmodbusdevicesdialog.h"

#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QVBoxLayout>
#include <vector>

SearchModbusDevicesDialog::SearchModbusDevicesDialog(QWidget *parent) : QDialog(parent)
{
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
    auto startAddrSelect = WDFunc::NewSPB2(addrGroupBox, "startAddrSelect", 0, 255, 0);
    widgets.startAddr = startAddrSelect;
    firstItemLayout->addWidget(startAddrLabel);
    firstItemLayout->addWidget(startAddrSelect);
    auto secondItemLayout = new QHBoxLayout;
    auto endAddrLabel = WDFunc::NewLBL2(addrGroupBox, "Конец:");
    auto endAddrSelect = WDFunc::NewSPB2(addrGroupBox, "endAddrSelect", 0, 255, 0);
    endAddrSelect->setValue(255);
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
    std::size_t index = 0;
    auto parityCheckBox1 = WDFunc::NewChB2(parityGroupBox, "parityCheckBox1", "Нет");
    parityMainLayout->addWidget(parityCheckBox1);
    widgets.parity[index++] = parityCheckBox1;
    auto parityCheckBox2 = WDFunc::NewChB2(parityGroupBox, "parityCheckBox2", "Нечёт");
    parityMainLayout->addWidget(parityCheckBox2);
    widgets.parity[index++] = parityCheckBox2;
    auto parityCheckBox3 = WDFunc::NewChB2(parityGroupBox, "parityCheckBox3", "Чёт");
    parityMainLayout->addWidget(parityCheckBox3);
    widgets.parity[index] = parityCheckBox3;
    auto buttonsLayout = new QHBoxLayout;
    auto selectAllButton = WDFunc::NewPB(parityGroupBox, "selectAllParityButton", //
        "Выбрать все", this, [this] {
            for (auto &checkBox : widgets.parity)
                checkBox->setCheckState(Qt::CheckState::Checked);
        });
    buttonsLayout->addWidget(selectAllButton);
    auto clearAllButton = WDFunc::NewPB(parityGroupBox, "clearAllParityButton", //
        "Очистить все", this, [this] {
            for (auto &checkBox : widgets.parity)
                checkBox->setCheckState(Qt::CheckState::Unchecked);
        });
    buttonsLayout->addWidget(clearAllButton);
    parityGroupBoxLayout->addLayout(parityMainLayout);
    parityGroupBoxLayout->addLayout(buttonsLayout);
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
    widgets.stopBits[0] = stopbitCheckBox1;
    auto stopbitCheckBox2 = WDFunc::NewChB2(stopbitsGroupBox, "stopbitCheckBox2", "2");
    stopbitsMainLayout->addWidget(stopbitCheckBox2);
    widgets.stopBits[1] = stopbitCheckBox2;
    auto buttonsLayout = new QHBoxLayout;
    auto selectAllButton = WDFunc::NewPB(stopbitsGroupBox, "selectAllStopbitButton", //
        "Выбрать все", this, [this] {
            for (auto &checkBox : widgets.stopBits)
                checkBox->setCheckState(Qt::CheckState::Checked);
        });
    buttonsLayout->addWidget(selectAllButton);
    auto clearAllButton = WDFunc::NewPB(stopbitsGroupBox, "clearAllStopbitButton", //
        "Очистить все", this, [this] {
            for (auto &checkBox : widgets.stopBits)
                checkBox->setCheckState(Qt::CheckState::Unchecked);
        });
    buttonsLayout->addWidget(clearAllButton);
    stopbitsGroupBoxLayout->addLayout(stopbitsMainLayout);
    stopbitsGroupBoxLayout->addLayout(buttonsLayout);
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

void SearchModbusDevicesDialog::runSearch()
{
    enum class Parity : quint8
    {
        None = 0,
        Odd,
        Even
    };

    [[maybe_unused]] auto comString = widgets.com->currentText();
    [[maybe_unused]] auto timeout = static_cast<int>(widgets.timeout->value());
    [[maybe_unused]] auto startAddr = static_cast<int>(widgets.startAddr->value());
    [[maybe_unused]] auto endAddr = static_cast<int>(widgets.endAddr->value());
    std::vector<int> bauds, stopbits;
    bauds.reserve(widgets.baud.size());
    stopbits.reserve(widgets.stopBits.size());
    std::vector<Parity> parities;
    parities.reserve(widgets.parity.size());

    bool ok = true;
    for (auto &checkBox : widgets.baud)
    {
        if (checkBox->isChecked())
        {
            auto baud = checkBox->text().toInt(&ok);
            if (ok)
                bauds.push_back(baud);
        }
    }
}
