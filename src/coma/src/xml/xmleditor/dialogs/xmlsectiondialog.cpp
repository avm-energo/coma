#include "xml/xmleditor/dialogs/xmlsectiondialog.h"

XmlSectionDialog::XmlSectionDialog(QWidget *parent) : XmlDialog(parent) { }

void XmlSectionDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto nameLayout = new QHBoxLayout;
    auto orderLayout = new QHBoxLayout;
    m_title += "раздела";

    // Виджеты для имени раздела
    auto nameLabel = new QLabel("Имя раздела: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(
        nameInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSectionDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    m_dlgItems.append(nameInput);

    // Приоритет
    auto orderLabel = new QLabel("Приоритет (0-без приоритета): ", this);
    auto orderInput = new QSpinBox(this);
    orderInput->setMinimum(configCountMin);
    orderInput->setMaximum(configCountMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(orderInput, &QSpinBox::textChanged,                  //
        this, qOverload<const QString &>(&XmlSectionDialog::dataChanged));
#endif
    QObject::connect(orderInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlSectionDialog::dataChanged));
    orderLayout->addWidget(orderLabel);
    orderLayout->addWidget(orderInput);
    m_dlgItems.append(orderInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(orderLayout);
}
