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
    auto sigIdInput = new QSpinBox(this);
    sigIdInput->setMinimum(idMin);
    sigIdInput->setMaximum(idMax);
    QObject::connect(sigIdInput, &QSpinBox::textChanged, //
        this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    QObject::connect(sigIdInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlModbusDialog::dataChanged));
    sigIdLayout->addWidget(sigIdLabel);
    sigIdLayout->addWidget(sigIdInput);
    dlgItems.append(sigIdInput);

    // Виджеты для типа регистра
    auto regTypeLabel = new QLabel("Тип регистра: ", this);
    auto regTypeInput = new QLineEdit("", this);
    QObject::connect(
        regTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    regTypeLayout->addWidget(regTypeLabel);
    regTypeLayout->addWidget(regTypeInput);
    dlgItems.append(regTypeInput);

    // Виджеты для типа возвращаемого значения
    auto typeLabel = new QLabel("Тип передаваемого значения: ", this);
    auto typeInput = new QComboBox(this);
    typeInput->addItems({ "uint32", "float", "bool" });
    // typeInput->setEditable(true);
    typeInput->setCurrentIndex(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeInput);
    QObject::connect(                                               //
        typeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlModbusDialog::dataChanged));       //
    dlgItems.append(typeInput);

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
