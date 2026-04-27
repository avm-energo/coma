#include <avm-gen/integers.h>
#include <avm-gen/settings.h>
#include <avm-gen/xml/xmlparse.h>
#include <avm-widgets/cbfunc.h>
#include <avm-widgets/chbfunc.h>
#include <avm-widgets/lefunc.h>
#include <avm-widgets/spbfunc.h>
#include <avm-widgets/wdfunc.h>
#include <xml/xmleditor/dialogs/xmloverlaydialog.h>
#include <xml/xmleditor/models/xmldatamodel.h>
#include <xml/xmltags.h>

const QStringList XmlOverlayDialog::s_dataTypes {
    "BYTE",
    "BYTE[3]",
    "BYTE[4]",
    "BYTE[6]",
    "BYTE[8]",
    "BYTE[16]",
    "BYTE[32]", //
    "WORD",
    "WORD[3]",
    "WORD[4]",
    "WORD[6]",
    "WORD[8]",
    "WORD[16]",
    "WORD[32]", //
    "DWORD",
    "DWORD[4]",
    "DWORD[6]",
    "DWORD[8]",
    "DWORD[16]",
    "DWORD[32]", //
    "FLOAT",
    "FLOAT[2]",
    "FLOAT[3]",
    "FLOAT[4]",
    "FLOAT[6]",
    "FLOAT[8]", //
    "CONF_DENS",
    "CONF_DENS[3]",
    "BYTE[]",
    "INT32", //
};

const QStringList XmlOverlayDialog::s_widgetTypes {
    "None",
    "IpControl",
    "CheckBoxGroup",
    "DoubleSpinBoxGroup", //
    "ComboBoxGroup",
    "QDoubleSpinBox",
    "QCheckBox",
    "QComboBox",        //
    "QTableView",
    "GasDensityWidget", //
};

XmlOverlayDialog::XmlOverlayDialog(QWidget *parent)
    : XmlDialog(parent)
    , m_widgetEdit(new QGroupBox("Редактирование виджета", this))
{
    m_widgetEdit->hide();
    m_values.clear();
}

void XmlOverlayDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 200);

    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto nameLayout = new QHBoxLayout;
    auto datatypeLayout = new QHBoxLayout;
    auto dtypeLayout = new QHBoxLayout;
    m_title += "оверлея";

    // Виджеты для ID записи
    idLayout->addWidget(SPBFunc::newLBL(this, "ID записи: ", "idspb", idMin, idMax, 0));
    QObject::connect(
        SPBFunc::spinBox(this, "idspb"), &EDoubleSpinBox::valueChanged, this, &XmlOverlayDialog::idChanged);
    mainLayout->addLayout(idLayout);
    m_dlgItems.append(SPBFunc::spinBox(this, "idspb"));

    // Виджеты для имени записи
    nameLayout->addWidget(LEFunc::newLBL(this, "Имя записи: ", "namele", true));
    QObject::connect(LEFunc::lineEdit(this, "namele"), &QLineEdit::textEdited, this, &XmlOverlayDialog::nameChanged);
    mainLayout->addLayout(nameLayout);
    m_dlgItems.append(LEFunc::lineEdit(this, "namele"));

    // Виджеты для типа данных записи
    datatypeLayout->addWidget(CBFunc::newLBL(this, "Тип передачи: ", "datatypecb", s_dataTypes));
    CBFunc::setIndex(this, "datatypecb", 0);
    QObject::connect(CBFunc::comboBox(this, "datatypecb"), &QComboBox::currentIndexChanged, this,
        &XmlOverlayDialog::datatypeChanged);
    mainLayout->addLayout(datatypeLayout);
    m_dlgItems.append(CBFunc::comboBox(this, "datatypecb"));

    dtypeLayout->addWidget(ChBFunc::New(this, "dtypechb", "Используется только для наладки: "));
#if (QT_VERSION < QT_VERSION_CHECK(6, 8, 2))
    QObject::connect(
        ChBFunc::checkBox(this, "dtypechb"), &QCheckBox::stateChanged, this, &XmlOverlayDialog::dtypeChanged);
#else
    QObject::connect(
        ChBFunc::checkBox(this, "dtypechb"), &QCheckBox::checkStateChanged, this, &XmlOverlayDialog::dtypeChanged);
#endif
    mainLayout->addLayout(dtypeLayout);
    m_dlgItems.append(ChBFunc::checkBox(this, "dtypechb"));

    // Включение/отключение добавления узла <widget> в запись
    auto widgetFlag = new QCheckBox("Имеет виджет для отображения: ", this);
    widgetFlag->setCheckState(Qt::Unchecked);
    mainLayout->addWidget(widgetFlag);
    m_dlgItems.append(widgetFlag);

#if (QT_VERSION < QT_VERSION_CHECK(6, 8, 2))
    QObject::connect(widgetFlag, &QCheckBox::stateChanged, this, //
        [this, mainLayout](int state)
#else
    QObject::connect(widgetFlag, &QCheckBox::checkStateChanged, this, //
        [this, mainLayout](const Qt::CheckState state)
#endif
        {
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

void XmlOverlayDialog::createWidgetEditBox()
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
        &XmlOverlayDialog::classnameChanged);
    classnameLayout->addWidget(classnameLabel);
    classnameLayout->addWidget(classnameInput);
    widgetLayout->addLayout(classnameLayout);
    m_dlgItems.append(classnameInput);

    // Виджеты для описания типа виджета записи
    auto widgetTypeLabel = new QLabel("Тип виджета: ", this);
    auto widgetTypeInput = new QComboBox(this);
    widgetTypeInput->addItems(s_widgetTypes);
    widgetTypeInput->setCurrentIndex(0);
    QObject::connect(widgetTypeInput, &QComboBox::currentIndexChanged, this, &XmlOverlayDialog::widgettypeChanged);
    widgetTypeLayout->addWidget(widgetTypeLabel);
    widgetTypeLayout->addWidget(widgetTypeInput);
    widgetLayout->addLayout(widgetTypeLayout);
    m_dlgItems.append(widgetTypeInput);

    loadS2TabsData();
    // Виджеты для ID вкладки виджета
    auto groupLabel = new QLabel("ID вкладки виджета: ", this);
    auto groupInput = new EComboBox(this);
    groupInput->setItems(m_s2TabsMap);
    QObject::connect(groupInput, &EComboBox::currentIndexChanged, this, &XmlOverlayDialog::tabidChanged);
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(groupInput);
    widgetLayout->addLayout(groupLayout);
    m_dlgItems.append(groupInput);

    // Виджеты для узла <string>
    auto stringLabel = new QLabel("Строка: ", this);
    auto stringInput = new QLineEdit("", this);
    QObject::connect(stringInput, &QLineEdit::textEdited, this, //
        &XmlOverlayDialog::stringChanged);
    stringLayout->addWidget(stringLabel);
    stringLayout->addWidget(stringInput);
    widgetLayout->addLayout(stringLayout);
    m_dlgItems.append(stringInput);

    // Виджеты для узла <tooltip>
    auto tooltipLabel = new QLabel("Тултип: ", this);
    auto tooltipInput = new QLineEdit("", this);
    QObject::connect(tooltipInput, &QLineEdit::textEdited, this, //
        &XmlOverlayDialog::tooltipChanged);
    tooltipLayout->addWidget(tooltipLabel);
    tooltipLayout->addWidget(tooltipInput);
    widgetLayout->addLayout(tooltipLayout);
    m_dlgItems.append(tooltipInput);

    // Виджеты для узла <parent>
    auto parentLabel = new QLabel("ID записи родительского виджета: ", this);
    auto parentInput = new QSpinBox(this);
    parentInput->setMinimum(0);
    parentInput->setMaximum(idMax);
    QObject::connect(parentInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, &XmlOverlayDialog::parentidChanged);
    parentLayout->addWidget(parentLabel);
    parentLayout->addWidget(parentInput);
    widgetLayout->addLayout(parentLayout);
    m_dlgItems.append(parentInput);

    // Виджеты для узла <count>
    auto countLabel = new QLabel("Количество: ", this);
    auto countInput = new QSpinBox(this);
    countInput->setMinimum(countMin);
    countInput->setMaximum(countMax);
    QObject::connect(countInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, &XmlOverlayDialog::countChanged);
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countInput);
    widgetLayout->addLayout(countLayout);
    m_dlgItems.append(countInput);

    constexpr auto int_min = std::numeric_limits<int>::min();
    constexpr auto int_max = std::numeric_limits<int>::max();
    // Виджеты для узла <min>
    auto minLabel = new QLabel("Минимальное значение: ", this);
    auto minInput = new QSpinBox(this);
    minInput->setMinimum(int_min);
    minInput->setMaximum(int_max);
    QObject::connect(minInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, &XmlOverlayDialog::minvalueChanged);
    minLayout->addWidget(minLabel);
    minLayout->addWidget(minInput);
    widgetLayout->addLayout(minLayout);
    m_dlgItems.append(minInput);

    // Виджеты для узла <max>
    auto maxLabel = new QLabel("Максимальное значение: ", this);
    auto maxInput = new QSpinBox(this);
    maxInput->setMinimum(int_min);
    maxInput->setMaximum(int_max);
    QObject::connect(maxInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, &XmlOverlayDialog::maxvalueChanged);
    maxLayout->addWidget(maxLabel);
    maxLayout->addWidget(maxInput);
    widgetLayout->addLayout(maxLayout);
    m_dlgItems.append(maxInput);

    // Виджеты для узла <decimals>
    auto decimalsLabel = new QLabel("Кол-во знаков после запятой: ", this);
    auto decimalsInput = new QSpinBox(this);
    decimalsInput->setMinimum(0);
    decimalsInput->setMaximum(32);
    QObject::connect(decimalsInput, qOverload<int>(&QSpinBox::valueChanged), this, &XmlOverlayDialog::decimalsChanged);
    decimalsLayout->addWidget(decimalsLabel);
    decimalsLayout->addWidget(decimalsInput);
    widgetLayout->addLayout(decimalsLayout);
    m_dlgItems.append(decimalsInput);

    // Виджеты для узла <field>
    auto fieldLabel = new QLabel("Заполнитель: ", this);
    auto fieldInput = new QLineEdit("", this);
    QObject::connect(fieldInput, &QLineEdit::textEdited, this, &XmlOverlayDialog::fieldChanged);
    fieldLayout->addWidget(fieldLabel);
    fieldLayout->addWidget(fieldInput);
    widgetLayout->addLayout(fieldLayout);
    m_dlgItems.append(fieldInput);

    // Виджеты для узла <string-array>
    auto strArrLabel = new QLabel("String Array: ", this);
    auto strArrInput = new QLineEdit("", this);
    QObject::connect(strArrInput, &QLineEdit::textEdited, this, &XmlOverlayDialog::strarrayChanged);
    strArrLayout->addWidget(strArrLabel);
    strArrLayout->addWidget(strArrInput);
    widgetLayout->addLayout(strArrLayout);
    m_dlgItems.append(strArrInput);

    m_widgetEdit->setLayout(widgetLayout);
}

void XmlOverlayDialog::parseConfigTab(const QDomNode &tabNode)
{
    int index = XmlParse::parseNumFromNode<u32>(tabNode, tags::id);
    QString desc = XmlParse::parseString(tabNode, tags::name);
    m_s2TabsMap[desc] = index;
}

void XmlOverlayDialog::idChanged(double value)
{
    m_values["id"] = value;
}

void XmlOverlayDialog::nameChanged(const QString &name)
{
    m_values["name"] = name;
}

void XmlOverlayDialog::datatypeChanged(const int &index)
{
    if (index < s_dataTypes.size())
        m_values["type"] = s_dataTypes[index];
}

void XmlOverlayDialog::dtypeChanged(Qt::CheckState state)
{
    m_values["dtype"] = state == Qt::Checked;
}

void XmlOverlayDialog::classnameChanged(const QString &classname)
{
    m_values["class"] = classname;
}

void XmlOverlayDialog::widgettypeChanged(int index)
{
    if (index < s_widgetTypes.size())
        m_values["wtype"] = s_widgetTypes[index];
}

void XmlOverlayDialog::tabidChanged(const int &id)
{
    m_values["group"] = id;
}

void XmlOverlayDialog::stringChanged(const QString &string)
{
    m_values["string"] = string;
}

void XmlOverlayDialog::tooltipChanged(const QString &string)
{
    m_values["tooltip"] = string;
}

void XmlOverlayDialog::parentidChanged(const int &id)
{
    m_values["parent"] = id;
}

void XmlOverlayDialog::countChanged(const int &count)
{
    m_values["count"] = count;
}

void XmlOverlayDialog::minvalueChanged(const int &value)
{
    m_values["min"] = value;
}

void XmlOverlayDialog::maxvalueChanged(const int &value)
{
    m_values["max"] = value;
}

void XmlOverlayDialog::decimalsChanged(const int &value)
{
    m_values["decimals"] = value;
}

void XmlOverlayDialog::fieldChanged(const QString &string)
{
    m_values["field"] = string;
}

void XmlOverlayDialog::strarrayChanged(const QString &string)
{
    m_values["string-array"] = string;
}

// we have only "id" in response list
void XmlOverlayDialog::loadModelData(const QStringList &response)
{
    if (response.isEmpty())
    {
        qDebug() << "Overlay response is empty!";
        return;
    }
    // Находим минимальное количество, при несовпадении игнорируем
    auto size = std::min(response.count(), m_dlgItems.count());
    for (auto i = 0; i < size; i++)
    {
        auto &item = m_dlgItems[i];
        std::visit( //
            overloaded {
                [&](QLineEdit *widget) { //
                    widget->setText(response[i]);
                },
                [&](QComboBox *widget) { //
                    auto itemIndex = widget->findText(response[i]);
                    if (itemIndex != -1)
                        widget->setCurrentIndex(itemIndex);
                    else
                        widget->setCurrentText(response[i]);
                },
                [&](EComboBox *widget) { //
                    widget->setIndexByValue(response[i].toInt());
                },
                [&](QSpinBox *widget) { //
                    widget->setValue(response[i].toInt());
                },
                [&](EDoubleSpinBox *widget) { //
                    widget->setValue(response[i].toDouble());
                },
                [&](QCheckBox *widget) { //
                    auto state = (response[i] == "1") ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
                    widget->setCheckState(state);
                },
                },
            item);
    }
    resetChangeState();
}

bool XmlOverlayDialog::loadS2TabsData()
{
    QDir homeDir(Settings::configDir());
    auto filename = qvariant_cast<QString>("s2files.xml");
    auto moduleFile = new QFile(homeDir.filePath(filename), this);
    if (moduleFile->open(QIODevice::ReadOnly))
    {
        QDomDocument domDoc;
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
        QString errMsg = "";
        auto line = 0, column = 0;
        if (domDoc.setContent(moduleFile, &errMsg, &line, &column))
#else
        QDomDocument::ParseResult result = domDoc.setContent(moduleFile);
        if (result.errorMessage.isEmpty())
#endif
        {
            auto domElement = domDoc.documentElement();
            XmlParse::parseNode(domElement, tags::conf_tabs,
                [this](const QDomNode &tabNode)
                {
                    parseConfigTab(tabNode); //
                });
        }
        // Если QtXml парсер не смог корректно считать xml файл
        else
        {
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
            qWarning() << errMsg << " File: " << filename << " Line: " << line << " Column: " << column;
#else
            qWarning() << result.errorMessage << " File: " << filename << " Line: " << result.errorLine
                       << " Column: " << result.errorColumn;
#endif
            return false;
        }
        moduleFile->close();
    }
    moduleFile->deleteLater();
    return true;
}
