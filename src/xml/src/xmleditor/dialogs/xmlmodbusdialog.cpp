#include "xml/xmleditor/dialogs/xmlmodbusdialog.h"

namespace StringData
{
constexpr auto readCoils = "Read Coils (0x01)";
constexpr auto readHold = "Read Holding (0x03)";
constexpr auto readInput = "Read Input (0x04)";
constexpr auto writeCoil = "Write Single Coil (0x05)";
constexpr auto writeSingle = "Write Single (0x06)";
constexpr auto writeMutiple = "Write Multiple (0x10)";

constexpr auto rCoilsNum = "1";
constexpr auto rHoldNum = "3";
constexpr auto rInputNum = "4";
constexpr auto wCoilNum = "5";
constexpr auto wSingleNum = "6";
constexpr auto wMultyNub = "16";
}

constexpr auto regTypeIndex = 1;

XmlModbusDialog::XmlModbusDialog(QWidget *parent) : XmlDialog(parent)
{
}

QString XmlModbusDialog::convertStringToType(const QString &namedTransType)
{
    using namespace StringData;
    static const QHash<QString, QString> convertMap = {
        { readCoils, rCoilsNum },    //
        { readHold, rHoldNum },      //
        { readInput, rInputNum },    //
        { writeCoil, wCoilNum },     //
        { writeSingle, wSingleNum }, //
        { writeMutiple, wMultyNub }  //
    };
    return convertMap.value(namedTransType, readInput);
}

int XmlModbusDialog::convertTypeToIndex(const QString &transTypeNum)
{
    using namespace StringData;
    static const QHash<QString, int> convertMap = {
        { rCoilsNum, 0 },  //
        { rHoldNum, 1 },   //
        { rInputNum, 2 },  //
        { wCoilNum, 3 },   //
        { wSingleNum, 4 }, //
        { wMultyNub, 5 }   //
    };
    return convertMap.value(transTypeNum, 0);
}

void XmlModbusDialog::setupUI(QVBoxLayout *mainLayout)
{
    using namespace StringData;
    // Настройки окна (размер, положение)
    setupSizePos(650, 180);
    // Создание слоёв окна
    auto sigIdLayout = new QHBoxLayout;
    auto regTypeLayout = new QHBoxLayout;
    auto typeLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    m_title += "элемента Modbus";

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    auto sigIdInput = new QSpinBox(this);
    sigIdInput->setMinimum(idMin);
    sigIdInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(sigIdInput, &QSpinBox::textChanged, //
        this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
#endif
    QObject::connect(sigIdInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlModbusDialog::dataChanged));
    sigIdLayout->addWidget(sigIdLabel);
    sigIdLayout->addWidget(sigIdInput);
    m_dlgItems.append(sigIdInput);

    // Виджеты для типа регистра
    auto regTypeLabel = new QLabel("Тип регистра: ", this);
    auto regTypeInput = new QComboBox(this);
    regTypeInput->addItems({ readCoils, readHold, readInput, writeCoil, writeSingle, writeMutiple });
    regTypeInput->setCurrentIndex(0);
    QObject::connect(                                                  //
        regTypeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlModbusDialog::dataChanged));          //
    regTypeLayout->addWidget(regTypeLabel);
    regTypeLayout->addWidget(regTypeInput);
    m_dlgItems.append(regTypeInput);

    // Виджеты для типа возвращаемого значения
    auto typeLabel = new QLabel("Тип передаваемого значения: ", this);
    auto typeInput = new QComboBox(this);
    typeInput->addItems({ "uint32", "float", "bool" });
    typeInput->setCurrentIndex(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeInput);
    QObject::connect(                                               //
        typeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlModbusDialog::dataChanged));       //
    m_dlgItems.append(typeInput);

    // Виджеты для описания
    auto descLabel = new QLabel("Описание: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(
        descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    m_dlgItems.append(descInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(sigIdLayout);
    mainLayout->addLayout(regTypeLayout);
    mainLayout->addLayout(typeLayout);
    mainLayout->addLayout(descLayout);
}

QStringList XmlModbusDialog::collectData()
{
    auto retVal = XmlDialog::collectData();
    retVal[regTypeIndex] = convertStringToType(retVal[regTypeIndex]);
    return retVal;
}

void XmlModbusDialog::loadModelData(const QStringList &response)
{
    XmlDialog::loadModelData(response);
    auto regTypeInput = std::get<QComboBox *>(m_dlgItems[regTypeIndex]);
    regTypeInput->setCurrentIndex(convertTypeToIndex(response[regTypeIndex]));
    resetChangeState();
}
