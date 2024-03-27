#include "xmlhiddenwidgetdialog.h"

XmlHiddenWidgetDialog::XmlHiddenWidgetDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlHiddenWidgetDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 300);
    // Создание слоёв окна
    auto indexLayout = new QHBoxLayout;
    auto titleLayout = new QHBoxLayout;
    auto nameLayout = new QHBoxLayout;
    auto viewLayout = new QHBoxLayout;
    auto datatypeLayout = new QHBoxLayout;
    auto addrLayout = new QHBoxLayout;
    auto visibilityLayout = new QHBoxLayout;
    mTitle += "виджета раздела \"Секретные операции\"";

    // Виджеты для индекса данных
    auto indexLabel = new QLabel("Индекс поля внутри отправляемой структуры: ", this);
    auto indexInput = new QSpinBox(this);
    indexInput->setMinimum(1);
    indexInput->setMaximum(40);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(indexInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlHiddenWidgetDialog::dataChanged));
#endif
    QObject::connect(indexInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlHiddenWidgetDialog::dataChanged));
    indexLayout->addWidget(indexLabel);
    indexLayout->addWidget(indexInput);
    dlgItems.append(indexInput);

    // Отображаемое имя поля данных
    auto titleLabel = new QLabel("Отображаемое имя поля данных: ", this);
    auto titleInput = new QLineEdit("", this);
    QObject::connect(titleInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlHiddenWidgetDialog::dataChanged));
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(titleInput);
    dlgItems.append(titleInput);

    // Префикс имён виджетов в системе Qt
    auto nameLabel = new QLabel("Префикс имён виджетов в системе Qt: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(nameInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlHiddenWidgetDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    dlgItems.append(nameInput);

    // Виджеты для типа отображения
    auto viewLabel = new QLabel("Тип виджета для отображения: ", this);
    auto viewInput = new QComboBox(this);
    viewInput->addItems({ "LineEdit", "Version" });
    viewInput->setCurrentIndex(0);
    viewLayout->addWidget(viewLabel);
    viewLayout->addWidget(viewInput);
    QObject::connect(                                               //
        viewInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlHiddenWidgetDialog::dataChanged)); //
    dlgItems.append(viewInput);

    // Виджеты для типа данных
    auto datatypeLabel = new QLabel("Тип данных поля в отправляемой структуре: ", this);
    auto datatypeInput = new QComboBox(this);
    datatypeInput->addItems({ "uint32", "float32" });
    datatypeInput->setCurrentIndex(0);
    datatypeLayout->addWidget(datatypeLabel);
    datatypeLayout->addWidget(datatypeInput);
    QObject::connect(                                                   //
        datatypeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlHiddenWidgetDialog::dataChanged));     //
    dlgItems.append(datatypeInput);

    // Виджеты для адреса поля
    auto addrLabel = new QLabel("Адрес поля в устройстве: ", this);
    auto addrInput = new QSpinBox(this);
    addrInput->setMinimum(addrMin);
    addrInput->setMaximum(addrMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(addrInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlHiddenWidgetDialog::dataChanged));
#endif
    QObject::connect(addrInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlHiddenWidgetDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    dlgItems.append(addrInput);

    // Виджеты для описания видимости поля данных
    auto visibilityLabel = new QLabel("Видимость поля на вкладке: ", this);
    auto visibilityInput = new QComboBox(this);
    visibilityInput->addItems({ "true", "false" });
    visibilityInput->setCurrentIndex(0);
    visibilityLayout->addWidget(visibilityLabel);
    visibilityLayout->addWidget(visibilityInput);
    QObject::connect(                                                     //
        visibilityInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlHiddenWidgetDialog::dataChanged));       //
    dlgItems.append(visibilityInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(indexLayout);
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(viewLayout);
    mainLayout->addLayout(datatypeLayout);
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(visibilityLayout);
}
