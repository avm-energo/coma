#include "xml/xmleditor/dialogs/xmlbsiextdialog.h"

XmlBsiExtDialog::XmlBsiExtDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlBsiExtDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 160);
    // Создание слоёв окна
    auto addrLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    auto typeLayout = new QHBoxLayout;
    auto visibilityLayout = new QHBoxLayout;
    mTitle += "элемента BSI Ext";

    // Виджеты для адреса сигнализации
    auto addrLabel = new QLabel("Адрес элемента: ", this);
    auto addrInput = new QSpinBox(this);
    addrInput->setMinimum(addrMin);
    addrInput->setMaximum(addrMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(addrInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlBsiExtDialog::dataChanged));
#endif
    QObject::connect(addrInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlBsiExtDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    dlgItems.append(addrInput);

    // Виджеты для описания элемента
    auto descLabel = new QLabel("Описание элемента: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlBsiExtDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    dlgItems.append(descInput);

    // Виджеты для описания типа хранимых данных
    auto typeLabel = new QLabel("Тип данных элемента: ", this);
    auto typeInput = new QComboBox(this);
    typeInput->addItems({ "uint32", "float32", "time32", "time64", "string32", "version32" });
    typeInput->setCurrentIndex(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeInput);
    QObject::connect(                                               //
        typeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlBsiExtDialog::dataChanged));       //
    dlgItems.append(typeInput);

    // Виджеты для описания видимости элемента
    auto visibilityLabel = new QLabel("Видимость элемента: ", this);
    auto visibilityInput = new QComboBox(this);
    visibilityInput->addItems({ "true", "false" });
    visibilityInput->setCurrentIndex(0);
    visibilityLayout->addWidget(visibilityLabel);
    visibilityLayout->addWidget(visibilityInput);
    QObject::connect(                                                     //
        visibilityInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlBsiExtDialog::dataChanged));             //
    dlgItems.append(visibilityInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(descLayout);
    mainLayout->addLayout(typeLayout);
    mainLayout->addLayout(visibilityLayout);
}
