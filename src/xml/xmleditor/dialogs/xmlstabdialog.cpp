#include "xmlstabdialog.h"

XmlSTabDialog::XmlSTabDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlSTabDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 120);
    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto nameLayout = new QHBoxLayout;
    mTitle += "вкладки";

    // Виджеты для ID вкладки
    auto idLabel = new QLabel("ID вкладки: ", this);
    auto idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSTabDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    dlgItems.append(idInput);

    // Виджеты для имени вкладки
    auto nameLabel = new QLabel("Имя вкладки: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(nameInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSTabDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    dlgItems.append(nameInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(nameLayout);
}
