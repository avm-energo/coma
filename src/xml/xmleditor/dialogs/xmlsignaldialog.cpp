#include "xmlsignaldialog.h"

XmlSignalDialog::XmlSignalDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlSignalDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 160);
    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto addrLayout = new QHBoxLayout;
    auto countLayout = new QHBoxLayout;
    auto typeLayout = new QHBoxLayout;
    mTitle += "группы сигналов";

    // Виджеты для адресов группы сигналов
    auto addrLabel = new QLabel("Адрес сигнализации: ", this);
    auto addrInput = new QSpinBox(this);
    addrInput->setMinimum(addrMin);
    addrInput->setMaximum(addrMax);
    QObject::connect(addrInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    QObject::connect(addrInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlSignalDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    dlgItems.append(addrInput);

    // Виджеты для количества сигналов в группе
    auto countLabel = new QLabel("Количество: ", this);
    auto countInput = new QSpinBox(this);
    countInput->setMinimum(countMin);
    countInput->setMaximum(countMax);
    QObject::connect(countInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    QObject::connect(countInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlSignalDialog::dataChanged));
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countInput);
    dlgItems.append(countInput);

    // Виджеты для ID группы сигналов
    auto idLabel = new QLabel("ID вкладки: ", this);
    auto idInput = new QSpinBox(this);
    idInput->setMinimum(idMin);
    idInput->setMaximum(idMax);
    QObject::connect(idInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    QObject::connect(idInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlSignalDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    dlgItems.append(idInput);

    // Виджеты для типа возвращаемого значения
    auto typeLabel = new QLabel("Тип возвращаемого значения: ", this);
    auto typeInput = new QComboBox(this);
    typeInput->addItems({ "Float", "BitString", "SinglePoint" });
    // typeInput->setEditable(true);
    typeInput->setCurrentIndex(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeInput);
    QObject::connect(                                               //
        typeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlSignalDialog::dataChanged));       //
    dlgItems.append(typeInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(countLayout);
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(typeLayout);
}
