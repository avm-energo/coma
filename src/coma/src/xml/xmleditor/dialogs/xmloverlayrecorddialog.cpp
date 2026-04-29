#include <avm-gen/settings.h>
#include <avm-gen/xml/xmlparse.h>
#include <avm-widgets/wdfunc.h>
#include <xml/xmleditor/dialogs/xmloverlayrecorddialog.h>
#include <xml/xmleditor/models/xmldatamodel.h>
#include <xml/xmltags.h>

#include <limits>

const QStringList XmlOverlayRecordDialog::s_dataTypes {
    "BYTE",    "BYTE[3]",   "BYTE[4]",    "BYTE[6]",   "BYTE[8]",  "BYTE[16]", "BYTE[32]",
    "WORD",    "WORD[3]",   "WORD[4]",    "WORD[6]",   "WORD[8]",  "WORD[16]", "WORD[32]",
    "DWORD",   "DWORD[4]",  "DWORD[6]",   "DWORD[8]",  "DWORD[16]","DWORD[32]",
    "FLOAT",   "FLOAT[2]",  "FLOAT[3]",   "FLOAT[4]",  "FLOAT[6]", "FLOAT[8]",
    "CONF_DENS","CONF_DENS[3]","BYTE[]",  "INT32",
};

const QStringList XmlOverlayRecordDialog::s_widgetTypes {
    "None", "IpControl", "CheckBoxGroup", "DoubleSpinBoxGroup", "ComboBoxGroup",
    "QDoubleSpinBox", "QCheckBox", "QComboBox", "QTableView", "GasDensityWidget",
};

XmlOverlayRecordDialog::XmlOverlayRecordDialog(QWidget *parent)
    : XmlDialog(parent)
    , m_nameEdit(new QGroupBox("Переопределить имя", this))
    , m_dtypeEdit(new QGroupBox("Переопределить флаг наладки", this))
    , m_typeEdit(new QGroupBox("Переопределить тип данных", this))
    , m_widgetEdit(new QGroupBox("Переопределить виджет", this))
{
    m_nameEdit->setCheckable(true);
    m_nameEdit->setChecked(false);
    m_dtypeEdit->setCheckable(true);
    m_dtypeEdit->setChecked(false);
    m_typeEdit->setCheckable(true);
    m_typeEdit->setChecked(false);
    m_widgetEdit->setCheckable(true);
    m_widgetEdit->setChecked(false);
}

// m_dlgItems layout:
// [0]  QSpinBox    id
// [1]  QLineEdit   name
// [2]  QCheckBox   dtype flag
// [3]  QComboBox   datatype text
// [4]  QLineEdit   classname
// [5]  QComboBox   widgetType
// [6]  EComboBox   group
// [7]  QLineEdit   string
// [8]  QLineEdit   tooltip
// [9]  QSpinBox    parent
// [10] QSpinBox    count
// [11] QSpinBox    min
// [12] QSpinBox    max
// [13] QSpinBox    decimals
// [14] QLineEdit   field
// [15] QLineEdit   strArray

void XmlOverlayRecordDialog::setupUI(QVBoxLayout *mainLayout)
{
    m_title += "записи оверлея";

    // ID (обязательный)
    {
        auto idLayout = new QHBoxLayout;
        auto idLabel = new QLabel("ID записи: ", this);
        auto idInput = new QSpinBox(this);
        idInput->setMinimum(idMin);
        idInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QObject::connect(idInput, &QSpinBox::textChanged, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
#endif
        QObject::connect(idInput, qOverload<int>(&QSpinBox::valueChanged),
            this, qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        idLayout->addWidget(idLabel);
        idLayout->addWidget(idInput);
        mainLayout->addLayout(idLayout);
        m_dlgItems.append(idInput); // [0]
    }

    // Группа: имя [1]
    {
        auto nameLayout = new QHBoxLayout;
        auto nameLabel = new QLabel("Имя записи: ", this);
        auto nameInput = new QLineEdit("", this);
        QObject::connect(nameInput, &QLineEdit::textEdited, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
        nameLayout->addWidget(nameLabel);
        nameLayout->addWidget(nameInput);
        m_nameEdit->setLayout(nameLayout);
        mainLayout->addWidget(m_nameEdit);
        m_dlgItems.append(nameInput); // [1]
        QObject::connect(m_nameEdit, &QGroupBox::toggled, this, [this](bool) { dataChanged(); });
    }

    // Группа: dtype [2]
    {
        auto dtypeLayout = new QHBoxLayout;
        auto dtypeLabel = new QLabel("Только для наладки: ", this);
        auto dtypeCheck = new QCheckBox("", this);
#if (QT_VERSION < QT_VERSION_CHECK(6, 8, 2))
        QObject::connect(dtypeCheck, &QCheckBox::stateChanged, this,
            qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
#else
        QObject::connect(dtypeCheck, &QCheckBox::checkStateChanged, this,
            qOverload<const Qt::CheckState>(&XmlOverlayRecordDialog::dataChanged));
#endif
        dtypeLayout->addWidget(dtypeLabel);
        dtypeLayout->addWidget(dtypeCheck);
        m_dtypeEdit->setLayout(dtypeLayout);
        mainLayout->addWidget(m_dtypeEdit);
        m_dlgItems.append(dtypeCheck); // [2]
        QObject::connect(m_dtypeEdit, &QGroupBox::toggled, this, [this](bool) { dataChanged(); });
    }

    // Группа: тип данных [3]
    {
        auto typeLayout = new QHBoxLayout;
        auto typeLabel = new QLabel("Тип передачи: ", this);
        auto typeInput = new QComboBox(this);
        typeInput->addItems(s_dataTypes);
        typeInput->setCurrentIndex(0);
        QObject::connect(typeInput, qOverload<int>(&QComboBox::currentIndexChanged),
            this, qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        typeLayout->addWidget(typeLabel);
        typeLayout->addWidget(typeInput);
        m_typeEdit->setLayout(typeLayout);
        mainLayout->addWidget(m_typeEdit);
        m_dlgItems.append(typeInput); // [3]
        QObject::connect(m_typeEdit, &QGroupBox::toggled, this, [this](bool) { dataChanged(); });
    }

    // Группа: виджет [4..15]
    createWidgetEditBox(mainLayout);
}

void XmlOverlayRecordDialog::createWidgetEditBox(QVBoxLayout *mainLayout)
{
    auto widgetLayout = new QVBoxLayout;
    constexpr auto int_min = std::numeric_limits<int>::min();
    constexpr auto int_max = std::numeric_limits<int>::max();

    // classname [4]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Имя класса: ", this);
        auto input = new QLineEdit("", this);
        QObject::connect(input, &QLineEdit::textEdited, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [4]
    }

    // widgetType [5]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Тип виджета: ", this);
        auto input = new QComboBox(this);
        input->addItems(s_widgetTypes);
        input->setCurrentIndex(0);
        QObject::connect(input, qOverload<int>(&QComboBox::currentIndexChanged),
            this, qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [5]
    }

    // group [6]
    {
        loadS2TabsData();
        auto layout = new QHBoxLayout;
        auto label = new QLabel("ID вкладки виджета: ", this);
        auto input = new EComboBox(this);
        input->setItems(m_s2TabsMap);
        QObject::connect(input, &EComboBox::currentIndexChanged, this,
            qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [6]
    }

    // string [7]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Строка: ", this);
        auto input = new QLineEdit("", this);
        QObject::connect(input, &QLineEdit::textEdited, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [7]
    }

    // tooltip [8]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Тултип: ", this);
        auto input = new QLineEdit("", this);
        QObject::connect(input, &QLineEdit::textEdited, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [8]
    }

    // parent [9]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("ID записи родительского виджета: ", this);
        auto input = new QSpinBox(this);
        input->setMinimum(0);
        input->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QObject::connect(input, &QSpinBox::textChanged, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
#endif
        QObject::connect(input, qOverload<int>(&QSpinBox::valueChanged),
            this, qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [9]
    }

    // count [10]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Количество: ", this);
        auto input = new QSpinBox(this);
        input->setMinimum(countMin);
        input->setMaximum(countMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QObject::connect(input, &QSpinBox::textChanged, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
#endif
        QObject::connect(input, qOverload<int>(&QSpinBox::valueChanged),
            this, qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [10]
    }

    // min [11]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Минимальное значение: ", this);
        auto input = new QSpinBox(this);
        input->setMinimum(int_min);
        input->setMaximum(int_max);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QObject::connect(input, &QSpinBox::textChanged, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
#endif
        QObject::connect(input, qOverload<int>(&QSpinBox::valueChanged),
            this, qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [11]
    }

    // max [12]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Максимальное значение: ", this);
        auto input = new QSpinBox(this);
        input->setMinimum(int_min);
        input->setMaximum(int_max);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QObject::connect(input, &QSpinBox::textChanged, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
#endif
        QObject::connect(input, qOverload<int>(&QSpinBox::valueChanged),
            this, qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [12]
    }

    // decimals [13]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Кол-во знаков после запятой: ", this);
        auto input = new QSpinBox(this);
        input->setMinimum(0);
        input->setMaximum(32);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QObject::connect(input, &QSpinBox::textChanged, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
#endif
        QObject::connect(input, qOverload<int>(&QSpinBox::valueChanged),
            this, qOverload<int>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [13]
    }

    // field [14]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("Заполнитель: ", this);
        auto input = new QLineEdit("", this);
        QObject::connect(input, &QLineEdit::textEdited, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [14]
    }

    // strArray [15]
    {
        auto layout = new QHBoxLayout;
        auto label = new QLabel("String Array: ", this);
        auto input = new QLineEdit("", this);
        QObject::connect(input, &QLineEdit::textEdited, this,
            qOverload<const QString &>(&XmlOverlayRecordDialog::dataChanged));
        layout->addWidget(label);
        layout->addWidget(input);
        widgetLayout->addLayout(layout);
        m_dlgItems.append(input); // [15]
    }

    m_widgetEdit->setLayout(widgetLayout);
    mainLayout->addWidget(m_widgetEdit);
    QObject::connect(m_widgetEdit, &QGroupBox::toggled, this, [this](bool) { dataChanged(); });
}

// Формат сохранения (20 элементов):
// [0]  id                — visible col 0
// [1]  name              — visible col 1
// [2]  datatype text     — visible col 2
// [3]  dtype "0"/"1"     — visible col 3
// [4]  overrideName "0"/"1"
// [5]  overrideDtype "0"/"1"
// [6]  overrideType "0"/"1"
// [7..19]: S2RecordHideData (isEnabled=overrideWidget, classname, type, group, string,
//          tooltip, parent, count, min, max, decimals, field, array)

QStringList XmlOverlayRecordDialog::collectData()
{
    QStringList result;

    // Visible fields
    result.append(QString::number(std::get<QSpinBox *>(m_dlgItems[0])->value())); // [0] id
    result.append(std::get<QLineEdit *>(m_dlgItems[1])->text());                  // [1] name
    result.append(std::get<QComboBox *>(m_dlgItems[3])->currentText());           // [2] datatype
    result.append(std::get<QCheckBox *>(m_dlgItems[2])->isChecked() ? "1" : "0"); // [3] dtype

    // Override flags
    result.append(m_nameEdit->isChecked() ? "1" : "0");  // [4]
    result.append(m_dtypeEdit->isChecked() ? "1" : "0"); // [5]
    result.append(m_typeEdit->isChecked() ? "1" : "0");  // [6]

    // S2RecordHideData
    result.append(m_widgetEdit->isChecked() ? "1" : "0");                         // [7] isEnabled
    result.append(std::get<QLineEdit *>(m_dlgItems[4])->text());                   // [8] classname
    result.append(std::get<QComboBox *>(m_dlgItems[5])->currentText());            // [9] type
    result.append(QString::number(                                                  // [10] group
        std::get<EComboBox *>(m_dlgItems[6])->getValueByCurrentIndex()));
    result.append(std::get<QLineEdit *>(m_dlgItems[7])->text());                   // [11] string
    result.append(std::get<QLineEdit *>(m_dlgItems[8])->text());                   // [12] tooltip
    result.append(QString::number(std::get<QSpinBox *>(m_dlgItems[9])->value()));  // [13] parent
    result.append(QString::number(std::get<QSpinBox *>(m_dlgItems[10])->value())); // [14] count
    result.append(QString::number(std::get<QSpinBox *>(m_dlgItems[11])->value())); // [15] min
    result.append(QString::number(std::get<QSpinBox *>(m_dlgItems[12])->value())); // [16] max
    result.append(QString::number(std::get<QSpinBox *>(m_dlgItems[13])->value())); // [17] decimals
    result.append(std::get<QLineEdit *>(m_dlgItems[14])->text());                  // [18] field
    result.append(std::get<QLineEdit *>(m_dlgItems[15])->text());                  // [19] strArray

    return result;
}

void XmlOverlayRecordDialog::loadModelData(const QStringList &response)
{
    if (response.count() < 20)
    {
        resetChangeState();
        return;
    }

    bool ok = false;

    // Visible fields [0..3]
    auto idVal = response[0].toInt(&ok);
    std::get<QSpinBox *>(m_dlgItems[0])->setValue(ok ? idVal : 0);
    std::get<QLineEdit *>(m_dlgItems[1])->setText(response[1]);
    auto wtIdx = std::get<QComboBox *>(m_dlgItems[3])->findText(response[2]);
    std::get<QComboBox *>(m_dlgItems[3])->setCurrentIndex(wtIdx >= 0 ? wtIdx : 0);
    std::get<QCheckBox *>(m_dlgItems[2])->setChecked(response[3] == "1");

    // Override flags [4..6]
    m_nameEdit->setChecked(response[4] == "1");
    m_dtypeEdit->setChecked(response[5] == "1");
    m_typeEdit->setChecked(response[6] == "1");

    // S2RecordHideData [7..19]
    m_widgetEdit->setChecked(response[7] == "1");
    std::get<QLineEdit *>(m_dlgItems[4])->setText(response[8]);
    auto typeIdx = std::get<QComboBox *>(m_dlgItems[5])->findText(response[9]);
    std::get<QComboBox *>(m_dlgItems[5])->setCurrentIndex(typeIdx >= 0 ? typeIdx : 0);

    auto groupVal = response[10].toInt(&ok);
    std::get<EComboBox *>(m_dlgItems[6])->setIndexByValue(ok ? groupVal : 0);

    std::get<QLineEdit *>(m_dlgItems[7])->setText(response[11]);
    std::get<QLineEdit *>(m_dlgItems[8])->setText(response[12]);

    auto parentVal = response[13].toInt(&ok);
    std::get<QSpinBox *>(m_dlgItems[9])->setValue(ok ? parentVal : 0);
    auto countVal = response[14].toInt(&ok);
    std::get<QSpinBox *>(m_dlgItems[10])->setValue(ok ? countVal : 1);
    auto minVal = response[15].toInt(&ok);
    std::get<QSpinBox *>(m_dlgItems[11])->setValue(ok ? minVal : 0);
    auto maxVal = response[16].toInt(&ok);
    std::get<QSpinBox *>(m_dlgItems[12])->setValue(ok ? maxVal : 0);
    auto decimalsVal = response[17].toInt(&ok);
    std::get<QSpinBox *>(m_dlgItems[13])->setValue(ok ? decimalsVal : 0);
    std::get<QLineEdit *>(m_dlgItems[14])->setText(response[18]);
    std::get<QLineEdit *>(m_dlgItems[15])->setText(response[19]);

    resetChangeState();
}

void XmlOverlayRecordDialog::parseConfigTab(const QDomNode &tabNode)
{
    int idx = XmlParse::parseNumFromNode<u32>(tabNode, tags::id);
    QString desc = XmlParse::parseString(tabNode, tags::name);
    if (desc != STRINF)
        m_s2TabsMap[desc] = idx;
}

bool XmlOverlayRecordDialog::loadS2TabsData()
{
    QDir homeDir(Settings::configDir());
    auto filename = qvariant_cast<QString>("s2files.xml");
    auto moduleFile = new QFile(homeDir.filePath(filename), this);
    if (moduleFile->open(QIODevice::ReadOnly))
    {
        QDomDocument domDoc;
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
        QString errMsg;
        auto line = 0, column = 0;
        if (domDoc.setContent(moduleFile, &errMsg, &line, &column))
#else
        QDomDocument::ParseResult result = domDoc.setContent(moduleFile);
        if (result.errorMessage.isEmpty())
#endif
        {
            auto domElement = domDoc.documentElement();
            XmlParse::parseNode(domElement, tags::conf_tabs,
                [this](const QDomNode &tabNode) { parseConfigTab(tabNode); });
        }
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
