#include "xml/xmleditor/dialogs/xmlconfigdialog.h"

#include <libavm-widgets/emessagebox.h>
#include <xml/xmleditor/models/xmldatamodel.h>

#include <QComboBox>
#include <QCompleter>

XmlConfigDialog::XmlConfigDialog(QWidget *parent) : XmlDialog(parent) { }

void XmlConfigDialog::loadS2RecordsData(QComboBox *combo)
{
    const auto &names = XmlDataModel::s2RecordsNameMap();
    for (auto it = names.cbegin(); it != names.cend(); ++it)
        combo->addItem(XmlDataModel::s2RecordLabel(it.key()));

    // Поиск по имени или id: вводимый текст ищется в любом месте надписи "Имя [id]"
    combo->setEditable(true);
    combo->setInsertPolicy(QComboBox::NoInsert);
    auto completer = new QCompleter(combo->model(), combo);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    combo->setCompleter(completer);
}

bool XmlConfigDialog::checkDataBeforeSaving(const QStringList &savedData)
{
    if (savedData.isEmpty() || XmlDataModel::s2RecordIdFromLabel(savedData.first()) < 0)
    {
        EMessageBox::warning(this, "Выберите параметр из списка");
        return false;
    }
    return true;
}

void XmlConfigDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 200);
    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto defLayout = new QHBoxLayout;
    auto countLayout = new QHBoxLayout;
    auto orderLayout = new QHBoxLayout;
    auto visibLayout = new QHBoxLayout;
    m_title += "конфига";

    // Виджеты для выбора параметра конфига
    auto idLabel = new QLabel("Параметр: ", this);
    auto idInput = new QComboBox(this);
    loadS2RecordsData(idInput);
    QObject::connect(idInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlConfigDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    m_dlgItems.append(idInput);

    // Виджеты для значения по умолчанию конфига
    auto defLabel = new QLabel("Значение по умолчанию: ", this);
    auto defaultInput = new QLineEdit("", this);
    QObject::connect(
        defaultInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    defLayout->addWidget(defLabel);
    defLayout->addWidget(defaultInput);
    m_dlgItems.append(defaultInput);

    // Виджеты для уточнённого значения количества элементов конфига
    auto countLabel = new QLabel("Уточнённое кол-во: ", this);
    auto countInput = new QSpinBox(this);
    countInput->setMinimum(configCountMin);
    countInput->setMaximum(configCountMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(countInput, &QSpinBox::textChanged,                  //
        this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
#endif
    QObject::connect(countInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlConfigDialog::dataChanged));
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countInput);
    m_dlgItems.append(countInput);

    auto orderLabel = new QLabel("Приоритет (0-без приоритета): ", this);
    auto orderInput = new QSpinBox(this);
    orderInput->setMinimum(configCountMin);
    orderInput->setMaximum(configCountMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(orderInput, &QSpinBox::textChanged,                  //
        this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
#endif
    QObject::connect(orderInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlConfigDialog::dataChanged));
    orderLayout->addWidget(orderLabel);
    orderLayout->addWidget(orderInput);
    m_dlgItems.append(orderInput);

    // Виджеты для видимости конфигурационного параметра
    auto visibLabel = new QLabel("Видимость: ", this);
    auto visibInput = new QComboBox(this);
    visibInput->addItems({ "true", "false" });
    visibInput->setCurrentIndex(0);
    visibLayout->addWidget(visibLabel);
    visibLayout->addWidget(visibInput);
    QObject::connect(                                                //
        visibInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlConfigDialog::dataChanged));        //
    m_dlgItems.append(visibInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(defLayout);
    mainLayout->addLayout(countLayout);
    mainLayout->addLayout(orderLayout);
    mainLayout->addLayout(visibLayout);
}
