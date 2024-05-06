#include "xml/xmleditor/dialogs/xmlworkjourdialog.h"

XmlWorkJourDialog::XmlWorkJourDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlWorkJourDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 120);
    // Создание слоёв окна
    auto addrLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    m_title += "события рабочего журнала";

    // Виджеты для адреса события
    auto addrLabel = new QLabel("Адрес сигнализации: ", this);
    auto addrInput = new QSpinBox(this);
    addrInput->setMinimum(addrMin);
    addrInput->setMaximum(addrMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(addrInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlWorkJourDialog::dataChanged));
#endif
    QObject::connect(addrInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlWorkJourDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    m_dlgItems.append(addrInput);

    // Виджеты для описания события
    auto descLabel = new QLabel("Описание: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(
        descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlWorkJourDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    m_dlgItems.append(descInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(descLayout);
}
