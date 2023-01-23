#include "xmlsgroupdialog.h"

XmlSGroupDialog::XmlSGroupDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlSGroupDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto nameLayout = new QHBoxLayout;
    auto idLayout = new QHBoxLayout;
    mTitle += "группы";

    // Виджеты для имени группы
    auto nameLabel = new QLabel("Имя группы: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(
        nameInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSGroupDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    dlgItems.append(nameInput);

    // Виджеты для ID вкладки
    auto idLabel = new QLabel("ID вкладки: ", this);
    auto idTab = new QLineEdit("", this);
    idTab->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idTab, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSGroupDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idTab);
    dlgItems.append(idTab);

    // Добавляем слои на главный слой
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(idLayout);
}
