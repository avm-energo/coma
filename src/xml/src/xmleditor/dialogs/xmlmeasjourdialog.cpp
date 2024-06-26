#include "xml/xmleditor/dialogs/xmlmeasjourdialog.h"

XmlMeasJourDialog::XmlMeasJourDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlMeasJourDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 200);
    // Создание слоёв окна
    auto indexLayout = new QHBoxLayout;
    auto headLayout = new QHBoxLayout;
    auto typeLayout = new QHBoxLayout;
    auto visibLayout = new QHBoxLayout;
    m_title += "элемента журнала измерений";

    // Виджеты для индекса измерения
    auto indexLabel = new QLabel("Индекс измерения: ", this);
    auto idndexInput = new QSpinBox(this);
    idndexInput->setMinimum(0);
    idndexInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(idndexInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlMeasJourDialog::dataChanged));
#endif
    QObject::connect(idndexInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlMeasJourDialog::dataChanged));
    indexLayout->addWidget(indexLabel);
    indexLayout->addWidget(idndexInput);
    m_dlgItems.append(idndexInput);

    // Виджеты для описания заголовка измерения в журнале измерений
    auto headLabel = new QLabel("Заголовок измерения: ", this);
    auto headInput = new QLineEdit("", this);
    QObject::connect(headInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlMeasJourDialog::dataChanged));
    headLayout->addWidget(headLabel);
    headLayout->addWidget(headInput);
    m_dlgItems.append(headInput);

    // Виджеты для выбора типа элемента журнала измерений
    auto typeLabel = new QLabel("Тип данных: ", this);
    auto typeInput = new QComboBox(this);
    typeInput->addItems({ "uint32", "float32", "time32", "time64" });
    typeInput->setCurrentIndex(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeInput);
    QObject::connect(typeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlMeasJourDialog::dataChanged));                  //
    m_dlgItems.append(typeInput);

    // Виджеты для выбора видимости конфигурационного элемента журнала измерений
    auto visibLabel = new QLabel("Видимость: ", this);
    auto visibInput = new QComboBox(this);
    visibInput->addItems({ "true", "false" });
    visibInput->setCurrentIndex(0);
    visibLayout->addWidget(visibLabel);
    visibLayout->addWidget(visibInput);
    QObject::connect(                                                //
        visibInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlMeasJourDialog::dataChanged));      //
    m_dlgItems.append(visibInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(indexLayout);
    mainLayout->addLayout(headLayout);
    mainLayout->addLayout(typeLayout);
    mainLayout->addLayout(visibLayout);
}
