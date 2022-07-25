#include "xmlmodbusdialog.h"

XmlModbusDialog::XmlModbusDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlModbusDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 180);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto Layout1 = new QHBoxLayout(this);
    auto Layout2 = new QHBoxLayout(this);
    auto Layout3 = new QHBoxLayout(this);
    auto Layout4 = new QHBoxLayout(this);

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    Layout1->addWidget(sigIdLabel);
    sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    Layout1->addWidget(sigIdInput);

    // Виджеты для типа регистра
    auto regTypeLabel = new QLabel("Тип регистра: ", this);
    Layout2->addWidget(regTypeLabel);
    regTypeInput = new QLineEdit("", this);
    regTypeInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        regTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    Layout2->addWidget(regTypeInput);

    // Виджеты для типа возвращаемого значения
    auto typeLabel = new QLabel("Тип возвращаемого значения: ", this);
    Layout3->addWidget(typeLabel);
    type = new QComboBox(this);
    type->addItems({ "uint32", "float", "bool" });
    type->setEditable(true);
    type->setCurrentIndex(0);
    Layout3->addWidget(type);

    // Виджеты для описания
    auto descLabel = new QLabel("Описание: ", this);
    Layout4->addWidget(descLabel);
    descInput = new QLineEdit("", this);
    QObject::connect(
        descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    Layout4->addWidget(descInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание элемента Modbus");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 4);
        setWindowTitle("Редактирование элемента Modbus");
        sigIdInput->setText(selectedData[0]);
        regTypeInput->setText(selectedData[1]);
        type->setCurrentText(selectedData[2]);
        descInput->setText(selectedData[3]);
    }
    QObject::connect(
        type, &QComboBox::currentTextChanged, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    mainLayout->addLayout(Layout3);
    mainLayout->addLayout(Layout4);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlModbusDialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList XmlModbusDialog::collectData()
{
    return {
        sigIdInput->text(), regTypeInput->text(), //
        type->currentText(), descInput->text()    //
    };
}
