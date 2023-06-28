#include "searchmodbusdevicesdialog.h"

#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QVBoxLayout>
#include <variant>
#include <vector>

namespace detail
{
using widget = std::variant<QComboBox *, QDoubleSpinBox *, QCheckBox *>;
using widgetList = std::vector<widget>;
}

struct SearchDialogWidgets
{
    detail::widgetList list;
};

SearchModbusDevicesDialog::SearchModbusDevicesDialog(QWidget *parent)
    : QDialog(parent), widgets(new SearchDialogWidgets)
{
    setObjectName("rsSearchDevicesDialog");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Поиск устройств");
    widgets->list.reserve(20);
    setupUI();
}

void SearchModbusDevicesDialog::selectAllCheckBoxes()
{
    auto checkBoxes = findChildren<QCheckBox *>();
    for (auto &checkBox : checkBoxes)
        checkBox->setCheckState(Qt::CheckState::Checked);
}

void SearchModbusDevicesDialog::clearAllCheckBoxes()
{
    auto checkBoxes = findChildren<QCheckBox *>();
    for (auto &checkBox : checkBoxes)
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
    auto comGroupBox = new QGroupBox("COM to the search", this);
    auto comGroupBoxLayout = new QVBoxLayout;
    auto comSelect = WDFunc::NewCB2(comGroupBox, "comSelect", comPorts);
    comGroupBoxLayout->addWidget(comSelect);
    comGroupBox->setLayout(comGroupBoxLayout);
    return comGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createTimeoutGroupBox()
{
    // Timeout group box
    auto timeoutGroupBox = new QGroupBox("Timeout setting", this);
    auto timeoutGroupBoxLayout = new QHBoxLayout;
    auto timeoutSelect = WDFunc::NewSPB2(timeoutGroupBox, "timeoutSelect", 100, 5000, 0);
    auto msLabel = WDFunc::NewLBL2(timeoutGroupBox, "мс.");
    timeoutGroupBoxLayout->addWidget(timeoutSelect, Qt::AlignRight);
    timeoutGroupBoxLayout->addWidget(msLabel, Qt::AlignLeft);
    timeoutGroupBox->setLayout(timeoutGroupBoxLayout);
    return timeoutGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createBaudGroupBox()
{
    // Baud rate group box
    auto baudGroupBox = new QGroupBox("Baud rate option", this);
    auto baudGroupBoxLayout = new QVBoxLayout;
    auto baudGridLayout = new QGridLayout;
    auto baudCheckBox1 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox1", "2400");
    baudGridLayout->addWidget(baudCheckBox1, 0, 0, 1, 1);
    auto baudCheckBox2 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox2", "4800");
    baudGridLayout->addWidget(baudCheckBox2, 0, 1, 1, 1);
    auto baudCheckBox3 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox3", "9600");
    baudGridLayout->addWidget(baudCheckBox3, 0, 2, 1, 1);
    auto baudCheckBox4 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox4", "19200");
    baudGridLayout->addWidget(baudCheckBox4, 1, 0, 1, 1);
    auto baudCheckBox5 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox5", "38400");
    baudGridLayout->addWidget(baudCheckBox5, 1, 1, 1, 1);
    auto baudCheckBox6 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox6", "57600");
    baudGridLayout->addWidget(baudCheckBox6, 1, 2, 1, 1);
    auto baudCheckBox7 = WDFunc::NewChB2(baudGroupBox, "baudCheckBox7", "115200");
    baudGridLayout->addWidget(baudCheckBox7, 2, 0, 1, 1);
    auto buttonsLayout = new QHBoxLayout;
    auto selectAllButton = WDFunc::NewPB(baudGroupBox, "selectAllButton", "Select All", //
        this, &SearchModbusDevicesDialog::selectAllCheckBoxes);
    buttonsLayout->addWidget(selectAllButton);
    auto clearAllButton = WDFunc::NewPB(baudGroupBox, "clearAllButton", "Clear All", //
        this, &SearchModbusDevicesDialog::clearAllCheckBoxes);
    buttonsLayout->addWidget(clearAllButton);
    baudGroupBoxLayout->addLayout(baudGridLayout);
    baudGroupBoxLayout->addLayout(buttonsLayout);
    baudGroupBox->setLayout(baudGroupBoxLayout);
    return baudGroupBox;
}

void SearchModbusDevicesDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    // First widget row
    auto firstRow = new QHBoxLayout;
    firstRow->addWidget(createComGroupBox());
    firstRow->addWidget(createTimeoutGroupBox());
    // Second widget row
    auto secondRow = new QHBoxLayout;
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
