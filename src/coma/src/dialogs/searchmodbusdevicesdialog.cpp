#include "dialogs/searchmodbusdevicesdialog.h"
#include "const.h"
#include "interfaces/utils/utils.h"

#include <libavm-widgets/cbfunc.h>
#include <libavm-widgets/chbfunc.h>
#include <libavm-widgets/emessagebox.h>
#include <libavm-widgets/lblfunc.h>
#include <libavm-widgets/pbfunc.h>
#include <libavm-widgets/spbfunc.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QVBoxLayout>

SearchModbusDevicesDialog::SearchModbusDevicesDialog(InterfaceSerialDialog *targetDialog,
    QWidget *parent) :
    QDialog(parent)
    , m_targetDialog(targetDialog)
{
    m_data.bauds.reserve(m_widgets.baud.size());
    m_data.parities.reserve(3);
    m_data.stopBits.reserve(2);
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

    for (const auto &portInfo : std::as_const(portList))
        comPorts << portInfo.portName();

    auto comGroupBox = new QGroupBox("Порт", this);
    auto comGroupBoxLayout = new QVBoxLayout;
    auto portSelect = CBFunc::New(comGroupBox, "portSelect", comPorts);

    m_widgets.port = portSelect;
    comGroupBoxLayout->addWidget(portSelect);
    comGroupBox->setLayout(comGroupBoxLayout);

    return comGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createTimeoutGroupBox()
{
    auto timeoutGroupBox = new QGroupBox("Таймаут", this);
    auto timeoutGroupBoxLayout = new QHBoxLayout;
    auto timeoutSelect = SPBFunc::New(timeoutGroupBox, "timeoutSelect", 500, 5000, 0);
    auto msLabel = LBLFunc::New(timeoutGroupBox, "мс.");

    m_widgets.timeout = timeoutSelect;
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
    auto startAddrLabel = LBLFunc::New(addrGroupBox, "Начало:");
    auto startAddrSelect = SPBFunc::New(addrGroupBox, "startAddrSelect", 1, 254, 0);

    m_widgets.startAddr = startAddrSelect;
    firstItemLayout->addWidget(startAddrLabel);
    firstItemLayout->addWidget(startAddrSelect);

    auto secondItemLayout = new QHBoxLayout;
    auto endAddrLabel = LBLFunc::New(addrGroupBox, "Конец:");
    auto endAddrSelect = SPBFunc::New(addrGroupBox, "endAddrSelect", 1, 254, 0);

    endAddrSelect->setValue(254);
    m_widgets.endAddr = endAddrSelect;
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
    std::size_t index = 0, rows = 3;

    QStringList bauds = SerialUtils::allBaudValuesRS485();

    for (const auto &baud : bauds)
    {
        auto baudCheckBox = ChBFunc::New(baudGroupBox, QString("baudCheckBox%1").arg(index), baud);
        baudGridLayout->addWidget(baudCheckBox, index/rows, index%rows, 1, 1);
        m_widgets.baud[index++] = baudCheckBox;
    }

    auto buttonsLayout = new QHBoxLayout;
    auto selectAllButton = PBFunc::New(baudGroupBox, "selectAllBaudButton", //
        "Выбрать все", this,
        [this]
        {
            for (auto &checkBox : m_widgets.baud)
                checkBox->setCheckState(Qt::CheckState::Checked);
        });
    buttonsLayout->addWidget(selectAllButton);

    auto clearAllButton = PBFunc::New(baudGroupBox, "clearAllBaudButton", //
        "Очистить все", this,
        [this]
        {
            for (auto &checkBox : m_widgets.baud)
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
    auto parityGroupBox = new QGroupBox("Четность", this);
    auto parityGroupBoxLayout = new QVBoxLayout;
    auto parityMainLayout = new QHBoxLayout;
    auto parityCheckBox1 = ChBFunc::New(parityGroupBox, "parityCheckBox1", ParityRS485::noParity);

    parityMainLayout->addWidget(parityCheckBox1);
    m_widgets.parityNone = parityCheckBox1;

    auto parityCheckBox2 = ChBFunc::New(parityGroupBox, "parityCheckBox2", ParityRS485::oddParity);
    parityMainLayout->addWidget(parityCheckBox2);
    m_widgets.parityOdd = parityCheckBox2;

    auto parityCheckBox3 = ChBFunc::New(parityGroupBox, "parityCheckBox3", ParityRS485::evenParity);
    parityMainLayout->addWidget(parityCheckBox3);
    m_widgets.parityEven = parityCheckBox3;
    parityGroupBoxLayout->addLayout(parityMainLayout);
    parityGroupBox->setLayout(parityGroupBoxLayout);

    return parityGroupBox;
}

QGroupBox *SearchModbusDevicesDialog::createStopbitsGroupBox()
{
    auto stopbitsGroupBox = new QGroupBox("Стоп-биты", this);
    auto stopbitsGroupBoxLayout = new QVBoxLayout;
    auto stopbitsMainLayout = new QHBoxLayout;
    auto stopbitCheckBox1 = ChBFunc::New(stopbitsGroupBox, "stopbitCheckBox1", SerialUtils::stopBitsToString(QSerialPort::StopBits::OneStop));

    stopbitsMainLayout->addWidget(stopbitCheckBox1);
    m_widgets.stopBitOne = stopbitCheckBox1;

    auto stopbitCheckBox2 = ChBFunc::New(stopbitsGroupBox, "stopbitCheckBox2", SerialUtils::stopBitsToString(QSerialPort::StopBits::TwoStop));
    stopbitsMainLayout->addWidget(stopbitCheckBox2);
    m_widgets.stopBitTwo = stopbitCheckBox2;
    stopbitsGroupBoxLayout->addLayout(stopbitsMainLayout);
    stopbitsGroupBox->setLayout(stopbitsGroupBoxLayout);

    return stopbitsGroupBox;
}

QHBoxLayout *SearchModbusDevicesDialog::createControlLayout()
{
    auto controlLayout = new QHBoxLayout;
    auto runSearchButton = PBFunc::New(this, "runSearchButton", //
        "Поиск", this, [this] { runSearch(); });
    auto cancelButton = PBFunc::New(this, "cancelButton",       //
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
    m_data.bauds.clear();
    m_data.parities.clear();
    m_data.stopBits.clear();

    m_data.port = m_widgets.port->currentText();
    m_data.timeout = static_cast<int>(m_widgets.timeout->value());
    m_data.startAddr = static_cast<int>(m_widgets.startAddr->value());
    m_data.endAddr = static_cast<int>(m_widgets.endAddr->value());

    for (auto &checkBox : m_widgets.baud)
        if (checkBox->isChecked())
            m_data.bauds.push_back(static_cast<qint32>(checkBox->text().toInt()));

    if (m_widgets.parityNone->isChecked())
        m_data.parities.push_back(QSerialPort::Parity::NoParity);
    if (m_widgets.parityOdd->isChecked())
        m_data.parities.push_back(QSerialPort::Parity::OddParity);
    if (m_widgets.parityEven->isChecked())
        m_data.parities.push_back(QSerialPort::Parity::EvenParity);
    if (m_widgets.stopBitOne->isChecked())
        m_data.stopBits.push_back(QSerialPort::StopBits::OneStop);
    if (m_widgets.stopBitTwo->isChecked())
        m_data.stopBits.push_back(QSerialPort::StopBits::TwoStop);
}

bool SearchModbusDevicesDialog::validate()
{
    // Проверка диапазона адресов
    if (m_data.startAddr > m_data.endAddr)
    {
        EMessageBox::warning(this, "Начальный адрес должен быть меньше или равен конечному");
        return false;
    }
    // Проверка выбранных скоростей
    if (m_data.bauds.empty())
    {
        EMessageBox::warning(this, "Необходимо выбрать хотя бы одну скорость");
        return false;
    }
    // Проверка выбранных параметров чётности
    if (m_data.parities.empty())
    {
        EMessageBox::warning(this, "Необходимо выбрать хотя бы один параметр чётности");
        return false;
    }
    // Проверка выбранных параметров стоповых битов
    if (m_data.stopBits.empty())
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
        auto proccessDlg = new SearchProccessDialog(m_data, m_targetDialog, this);
        connect(proccessDlg, &SearchProccessDialog::deviceAddedSuccessfully, this, [this]() { close(); });
        proccessDlg->show();
        proccessDlg->search();
    }
}
