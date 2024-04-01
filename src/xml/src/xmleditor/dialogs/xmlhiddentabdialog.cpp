#include "xml/xmleditor/dialogs/xmlhiddentabdialog.h"

XmlHiddenTabDialog::XmlHiddenTabDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlHiddenTabDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 250);
    // Создание слоёв окна
    auto tabNameLayout = new QHBoxLayout;
    auto prefixNameLayout = new QHBoxLayout;
    auto flagLayout = new QHBoxLayout;
    auto backgroundLayout = new QHBoxLayout;

    m_title += "вкладки раздела \"Секретные операции\"";

    // Виджеты для имени вкладки
    auto tabNameLabel = new QLabel("Имя вкладки: ", this);
    auto tabNameInput = new QLineEdit("", this);
    QObject::connect(tabNameInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlHiddenTabDialog::dataChanged));
    tabNameLayout->addWidget(tabNameLabel);
    tabNameLayout->addWidget(tabNameInput);
    m_dlgItems.append(tabNameInput);

    // Виджеты для префикса
    auto prefixNameLabel = new QLabel("Префикс для имён виджетов на вкладке: ", this);
    auto prefixNameInput = new QLineEdit("", this);
    QObject::connect(prefixNameInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlHiddenTabDialog::dataChanged));
    prefixNameLayout->addWidget(prefixNameLabel);
    prefixNameLayout->addWidget(prefixNameInput);
    m_dlgItems.append(prefixNameInput);

    // Виджеты для флага данных
    auto flagLabel = new QLabel("Флаг данных: ", this);
    auto flagInput = new QSpinBox(this);
    flagInput->setMinimum(1);
    flagInput->setMaximum(8);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(flagInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlHiddenTabDialog::dataChanged));
#endif
    QObject::connect(flagInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlHiddenTabDialog::dataChanged));
    flagLayout->addWidget(flagLabel);
    flagLayout->addWidget(flagInput);
    m_dlgItems.append(flagInput);

    // Виджеты для заднего фона
    auto backgroundLabel = new QLabel("Задний фон вкладки: ", this);
    auto backgroundInput = new QLineEdit("", this);
    QObject::connect(backgroundInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlHiddenTabDialog::dataChanged));
    backgroundLayout->addWidget(backgroundLabel);
    backgroundLayout->addWidget(backgroundInput);
    m_dlgItems.append(backgroundInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(tabNameLayout);
    mainLayout->addLayout(prefixNameLayout);
    mainLayout->addLayout(flagLayout);
    mainLayout->addLayout(backgroundLayout);
}
