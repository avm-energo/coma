#include "xml/xmleditor/dialogs/xmlmwidgetdialog.h"

XmlMWidgetDialog::XmlMWidgetDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlMWidgetDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 300);
    // Создание слоёв окна
    auto addrLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    auto countLayout = new QHBoxLayout;
    auto tooltipLayout = new QHBoxLayout;
    auto viewLayout = new QHBoxLayout;
    auto strArrLayout = new QHBoxLayout;
    m_title += "описания мульти-виджета";

    // Виджеты для начального адреса
    auto addrLabel = new QLabel("Адрес сигнала: ", this);
    auto addrInput = new QSpinBox(this);
    addrInput->setMinimum(addrMin);
    addrInput->setMaximum(addrMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(addrInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
#endif
    QObject::connect(addrInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlMWidgetDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    m_dlgItems.append(addrInput);

    // Виджеты для имени мульти-виджета
    auto descLabel = new QLabel("Имя: ", this);
    auto desc = new QLineEdit("", this);
    QObject::connect(desc, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(desc);
    m_dlgItems.append(desc);

    // Виджеты для количества адресов
    auto countLabel = new QLabel("Количество: ", this);
    auto countInput = new QSpinBox(this);
    countInput->setMinimum(countMin);
    countInput->setMaximum(countMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(countInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
#endif
    QObject::connect(countInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlMWidgetDialog::dataChanged));
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countInput);
    m_dlgItems.append(countInput);

    // Виджеты для тултипа
    auto tooltipLabel = new QLabel("Тултип: ", this);
    auto tooltip = new QLineEdit("", this);
    QObject::connect(tooltip, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    tooltipLayout->addWidget(tooltipLabel);
    tooltipLayout->addWidget(tooltip);
    m_dlgItems.append(tooltip);

    // Виджеты для массива строк
    auto strArrLabel = new QLabel("String Array: ", this);
    auto strArray = new QLineEdit("", this);
    QObject::connect(
        strArray, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    strArrLayout->addWidget(strArrLabel);
    strArrLayout->addWidget(strArray);
    m_dlgItems.append(strArray);

    // Виджеты для типа отображения
    auto viewLabel = new QLabel("Тип отображения: ", this);
    auto viewInput = new QComboBox(this);
    viewInput->addItems({ "float", "bitset" });
    viewInput->setCurrentIndex(0);
    viewLayout->addWidget(viewLabel);
    viewLayout->addWidget(viewInput);
    QObject::connect(                                               //
        viewInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlMWidgetDialog::dataChanged));      //
    m_dlgItems.append(viewInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(descLayout);
    mainLayout->addLayout(countLayout);
    mainLayout->addLayout(tooltipLayout);
    mainLayout->addLayout(viewLayout);
    mainLayout->addLayout(strArrLayout);
}
