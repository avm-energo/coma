#include "xml/xmleditor/dialogs/xmlstabdialog.h"

XmlSectionTabDialog::XmlSectionTabDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlSectionTabDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 120);
    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto nameLayout = new QHBoxLayout;
    m_title += "вкладки";

    // Виджеты для ID вкладки
    auto idLabel = new QLabel("ID вкладки: ", this);
    auto idTabInput = new QSpinBox(this);
    idTabInput->setMinimum(idMin);
    idTabInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(idTabInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlSectionTabDialog::dataChanged));
#endif
    QObject::connect(idTabInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlSectionTabDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idTabInput);
    m_dlgItems.append(idTabInput);

    // Виджеты для имени вкладки
    auto nameLabel = new QLabel("Имя вкладки: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(nameInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlSectionTabDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    m_dlgItems.append(nameInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(nameLayout);
}
