#include "xml/xmleditor/dialogs/xmlsgroupdialog.h"

XmlSGroupDialog::XmlSGroupDialog(QWidget *parent) : XmlDialog(parent) { }

void XmlSGroupDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto nameLayout = new QHBoxLayout;
    auto idLayout = new QHBoxLayout;
    auto orderLayout = new QHBoxLayout;
    m_title += "группы";

    // Виджеты для имени группы
    auto nameLabel = new QLabel("Имя группы: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(
        nameInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSGroupDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    m_dlgItems.append(nameInput);

    // Виджеты для ID группы
    auto idLabel = new QLabel("ID вкладки: ", this);
    auto idInput = new QSpinBox(this);
    idInput->setMinimum(idMin);
    idInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(idInput, &QSpinBox::textChanged, this,            //
        qOverload<const QString &>(&XmlSGroupDialog::dataChanged));
#endif
    QObject::connect(idInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlSGroupDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    m_dlgItems.append(idInput);

    // Приоритет
    auto orderLabel = new QLabel("Приоритет (0-без приоритета): ", this);
    auto orderInput = new QSpinBox(this);
    orderInput->setMinimum(configCountMin);
    orderInput->setMaximum(configCountMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(orderInput, &QSpinBox::textChanged,                  //
        this, qOverload<const QString &>(&XmlSGroupDialog::dataChanged));
#endif
    QObject::connect(orderInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlSGroupDialog::dataChanged));
    orderLayout->addWidget(orderLabel);
    orderLayout->addWidget(orderInput);
    m_dlgItems.append(orderInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(orderLayout);
}
