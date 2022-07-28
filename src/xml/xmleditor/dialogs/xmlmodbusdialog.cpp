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
    auto sigIdLayout = new QHBoxLayout;
    auto regTypeLayout = new QHBoxLayout;
    auto typeLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    sigIdLayout->addWidget(sigIdLabel);
    sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    sigIdLayout->addWidget(sigIdInput);

    // Виджеты для типа регистра
    auto regTypeLabel = new QLabel("Тип регистра: ", this);
    regTypeLayout->addWidget(regTypeLabel);
    regTypeInput = new QLineEdit("", this);
    regTypeInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        regTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    regTypeLayout->addWidget(regTypeInput);

    // Виджеты для типа возвращаемого значения
    auto typeLabel = new QLabel("Тип возвращаемого значения: ", this);
    typeLayout->addWidget(typeLabel);
    type = new QComboBox(this);
    type->addItems({ "uint32", "float", "bool" });
    type->setEditable(true);
    type->setCurrentIndex(0);
    typeLayout->addWidget(type);

    // Виджеты для описания
    auto descLabel = new QLabel("Описание: ", this);
    descLayout->addWidget(descLabel);
    descInput = new QLineEdit("", this);
    QObject::connect(
        descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));
    descLayout->addWidget(descInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setTitle("Создание элемента Modbus");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 4);
        setTitle("Редактирование элемента Modbus");
        sigIdInput->setText(selectedData[0]);
        regTypeInput->setText(selectedData[1]);
        type->setCurrentText(selectedData[2]);
        descInput->setText(selectedData[3]);
    }
    QObject::connect(
        type, &QComboBox::currentTextChanged, this, qOverload<const QString &>(&XmlModbusDialog::dataChanged));

    // Добавляем слои на главный слой
    mainLayout->addLayout(sigIdLayout);
    mainLayout->addLayout(regTypeLayout);
    mainLayout->addLayout(typeLayout);
    mainLayout->addLayout(descLayout);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList XmlModbusDialog::collectData()
{
    return {
        sigIdInput->text(), regTypeInput->text(), //
        type->currentText(), descInput->text()    //
    };
}
