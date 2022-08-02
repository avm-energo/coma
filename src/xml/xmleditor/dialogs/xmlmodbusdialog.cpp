#include "xmlmodbusdialog.h"

XmlModbusDialog::XmlModbusDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlModbusDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 180);
    // Создание слоёв окна
    auto sigIdLayout = new QHBoxLayout;
    auto regTypeLayout = new QHBoxLayout;
    auto typeLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    mTitle += "элемента Modbus";

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    auto sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    sigIdLayout->addWidget(sigIdLabel);
    sigIdLayout->addWidget(sigIdInput);
    dlgItems.append(sigIdInput);

    // Виджеты для типа регистра
    auto regTypeLabel = new QLabel("Тип регистра: ", this);
    auto regTypeInput = new QLineEdit("", this);
    regTypeInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        regTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    regTypeLayout->addWidget(regTypeLabel);
    regTypeLayout->addWidget(regTypeInput);
    dlgItems.append(regTypeInput);

    // Виджеты для типа возвращаемого значения
    auto typeLabel = new QLabel("Тип возвращаемого значения: ", this);
    type = new QComboBox(this);
    type->addItems({ "uint32", "float", "bool" });
    type->setEditable(true);
    type->setCurrentIndex(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(type);
    QObject::connect(
        type, SLOT(&QComboBox::currentIndexChanged), this, SIGNAL(qOverload<int>(&XmlModbusDialog::dataChanged)));

    // Виджеты для описания
    auto descLabel = new QLabel("Описание: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(
        descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    dlgItems.append(descInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(sigIdLayout);
    mainLayout->addLayout(regTypeLayout);
    mainLayout->addLayout(typeLayout);
    mainLayout->addLayout(descLayout);
}

QStringList XmlModbusDialog::collectData()
{
    auto retVal = XmlDialog::collectData();
    retVal.insert(2, type->currentText());
    return retVal;
}
