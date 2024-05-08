#include "xml/xmleditor/dialogs/xmls2recorddialog.h"

#include <limits>
#include <widgets/wd_func.h>

const QStringList XmlS2RecordDialog::s_dataTypes {
    "BYTE", "BYTE[4]", "BYTE[6]", "BYTE[8]", "BYTE[16]", "BYTE[32]",       //
    "WORD", "WORD[4]", "WORD[6]", "WORD[8]", "WORD[16]", "WORD[32]",       //
    "DWORD", "DWORD[4]", "DWORD[6]", "DWORD[8]", "DWORD[16]", "DWORD[32]", //
    "FLOAT", "FLOAT[2]", "FLOAT[3]", "FLOAT[4]", "FLOAT[6]", "FLOAT[8]",   //
    "CONF_DENS", "CONF_DENS[3]", "BYTE[]", "INT32",                        //
};

const QStringList XmlS2RecordDialog::s_widgetTypes {
    "None", "IpControl", "CheckBoxGroup", "DoubleSpinBoxGroup",   //
    "QComboBoxGroup", "QDoubleSpinBox", "QCheckBox", "QComboBox", //
    "QTableView", "GasDensityWidget",                             //
};

XmlS2RecordDialog::XmlS2RecordDialog(QWidget *parent)
    : XmlDialog(parent), m_widgetEdit(new QGroupBox("Редактирование виджета", this))
{
    m_widgetEdit->hide();
}

void XmlS2RecordDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 200);

    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto nameLayout = new QHBoxLayout;
    auto datatypeLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    m_title += "описания S2 записи";

    // Виджеты для ID записи
    auto idLabel = new QLabel("ID записи: ", this);
    auto idInput = new QSpinBox(this);
    idInput->setMinimum(idMin);
    idInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(idInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
#endif
    QObject::connect(idInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    mainLayout->addLayout(idLayout);
    m_dlgItems.append(idInput);

    // Виджеты для имени записи
    auto nameLabel = new QLabel("Имя записи: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(nameInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    mainLayout->addLayout(nameLayout);
    m_dlgItems.append(nameInput);

    // Виджеты для типа данных записи
    auto datatypeLabel = new QLabel("Тип передачи: ", this);
    auto datatypeInput = new QComboBox(this);
    datatypeInput->addItems(s_dataTypes);
    datatypeInput->setCurrentIndex(0);
    QObject::connect(                                                   //
        datatypeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));         //
    datatypeLayout->addWidget(datatypeLabel);
    datatypeLayout->addWidget(datatypeInput);
    mainLayout->addLayout(datatypeLayout);
    m_dlgItems.append(datatypeInput);

    // Виджеты для описания записи
    auto descLabel = new QLabel("Описание: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    mainLayout->addLayout(descLayout);
    m_dlgItems.append(descInput);

    // Включение/отключение добавления узла <widget> в запись
    auto widgetFlag = new QCheckBox("Имеет виджет для отображения: ", this);
    widgetFlag->setCheckState(Qt::Unchecked);
    mainLayout->addWidget(widgetFlag);
    m_dlgItems.append(widgetFlag);

    QObject::connect(widgetFlag, &QCheckBox::stateChanged, this, //
        [this, mainLayout](const int state) {
            if (state == Qt::Unchecked)
            {
                mainLayout->removeWidget(m_widgetEdit);
                setFixedSize(650, 200);
                m_widgetEdit->hide();
            }
            else if (state == Qt::Checked)
            {
                mainLayout->insertWidget(mainLayout->count() - 1, m_widgetEdit);
                setFixedSize(650, 600);
                m_widgetEdit->show();
            }
            dataChanged(state);
        });
    createWidgetEditBox();
}

void XmlS2RecordDialog::createWidgetEditBox()
{
    auto widgetLayout = new QVBoxLayout;
    auto classnameLayout = new QHBoxLayout;
    auto widgetTypeLayout = new QHBoxLayout;
    auto groupLayout = new QHBoxLayout;
    auto stringLayout = new QHBoxLayout;
    auto tooltipLayout = new QHBoxLayout;

    auto parentLayout = new QHBoxLayout;
    auto countLayout = new QHBoxLayout;
    auto minLayout = new QHBoxLayout;
    auto maxLayout = new QHBoxLayout;
    auto decimalsLayout = new QHBoxLayout;
    auto fieldLayout = new QHBoxLayout;
    auto strArrLayout = new QHBoxLayout;

    // Виджеты для описания имени класса
    auto classnameLabel = new QLabel("Имя класса: ", this);
    auto classnameInput = new QLineEdit("", this);
    QObject::connect(classnameInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    classnameLayout->addWidget(classnameLabel);
    classnameLayout->addWidget(classnameInput);
    widgetLayout->addLayout(classnameLayout);
    m_dlgItems.append(classnameInput);

    // Виджеты для описания типа виджета записи
    auto widgetTypeLabel = new QLabel("Тип виджета: ", this);
    auto widgetTypeInput = new QComboBox(this);
    widgetTypeInput->addItems(s_widgetTypes);
    widgetTypeInput->setCurrentIndex(0);
    QObject::connect(                                                     //
        widgetTypeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));           //
    widgetTypeLayout->addWidget(widgetTypeLabel);
    widgetTypeLayout->addWidget(widgetTypeInput);
    widgetLayout->addLayout(widgetTypeLayout);
    m_dlgItems.append(widgetTypeInput);

    // Виджеты для ID вкладки виджета
    auto groupLabel = new QLabel("ID вкладки виджета: ", this);
    auto groupInput = new QSpinBox(this);
    groupInput->setMinimum(0);
    groupInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(groupInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
#endif
    QObject::connect(groupInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(groupInput);
    widgetLayout->addLayout(groupLayout);
    m_dlgItems.append(groupInput);

    // Виджеты для узла <string>
    auto stringLabel = new QLabel("Строка: ", this);
    auto stringInput = new QLineEdit("", this);
    QObject::connect(stringInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    stringLayout->addWidget(stringLabel);
    stringLayout->addWidget(stringInput);
    widgetLayout->addLayout(stringLayout);
    m_dlgItems.append(stringInput);

    // Виджеты для узла <tooltip>
    auto tooltipLabel = new QLabel("Тултип: ", this);
    auto tooltipInput = new QLineEdit("", this);
    QObject::connect(tooltipInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    tooltipLayout->addWidget(tooltipLabel);
    tooltipLayout->addWidget(tooltipInput);
    widgetLayout->addLayout(tooltipLayout);
    m_dlgItems.append(tooltipInput);

    // Виджеты для узла <parent>
    auto parentLabel = new QLabel("ID записи родительского виджета: ", this);
    auto parentInput = new QSpinBox(this);
    parentInput->setMinimum(0);
    parentInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(parentInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
#endif
    QObject::connect(parentInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));
    parentLayout->addWidget(parentLabel);
    parentLayout->addWidget(parentInput);
    widgetLayout->addLayout(parentLayout);
    m_dlgItems.append(parentInput);

    // Виджеты для узла <count>
    auto countLabel = new QLabel("Количество: ", this);
    auto countInput = new QSpinBox(this);
    countInput->setMinimum(countMin);
    countInput->setMaximum(countMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(countInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
#endif
    QObject::connect(countInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countInput);
    widgetLayout->addLayout(countLayout);
    m_dlgItems.append(countInput);

    constexpr auto int_min = std::numeric_limits<int>::min();
    constexpr auto int_max = std::numeric_limits<int>::max();
    // Виджеты для узла <min>
    auto minLabel = new QLabel("Минимальное значение: ", this);
    auto minInput = new QSpinBox(this);
    countInput->setMinimum(int_min);
    countInput->setMaximum(int_max);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(minInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
#endif
    QObject::connect(minInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));
    minLayout->addWidget(minLabel);
    minLayout->addWidget(minInput);
    widgetLayout->addLayout(minLayout);
    m_dlgItems.append(minInput);

    // Виджеты для узла <max>
    auto maxLabel = new QLabel("Максимальное значение: ", this);
    auto maxInput = new QSpinBox(this);
    maxInput->setMinimum(idMin);
    maxInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(maxInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
#endif
    QObject::connect(maxInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));
    maxLayout->addWidget(maxLabel);
    maxLayout->addWidget(maxInput);
    widgetLayout->addLayout(maxLayout);
    m_dlgItems.append(maxInput);

    // Виджеты для узла <decimals>
    auto decimalsLabel = new QLabel("Кол-во знаков после запятой: ", this);
    auto decimalsInput = new QSpinBox(this);
    decimalsInput->setMinimum(0);
    decimalsInput->setMaximum(32);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(decimalsInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
#endif
    QObject::connect(decimalsInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));
    decimalsLayout->addWidget(decimalsLabel);
    decimalsLayout->addWidget(decimalsInput);
    widgetLayout->addLayout(decimalsLayout);
    m_dlgItems.append(decimalsInput);

    // Виджеты для узла <field>
    auto fieldLabel = new QLabel("Заполнитель: ", this);
    auto fieldInput = new QLineEdit("", this);
    QObject::connect(fieldInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    fieldLayout->addWidget(fieldLabel);
    fieldLayout->addWidget(fieldInput);
    widgetLayout->addLayout(fieldLayout);
    m_dlgItems.append(fieldInput);

    // Виджеты для узла <string-array>
    auto strArrLabel = new QLabel("String Array: ", this);
    auto strArrInput = new QLineEdit("", this);
    QObject::connect(strArrInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    strArrLayout->addWidget(strArrLabel);
    strArrLayout->addWidget(strArrInput);
    widgetLayout->addLayout(strArrLayout);
    m_dlgItems.append(strArrInput);

    m_widgetEdit->setLayout(widgetLayout);
}

void XmlS2RecordDialog::loadModelData(const QStringList &response)
{
    XmlDialog::loadModelData(response);
    resetChangeState();
}
