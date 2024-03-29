#include "xml/xmleditor/dialogs/xmlsectiondialog.h"

XmlSectionDialog::XmlSectionDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlSectionDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 80);
    // Создание слоёв окна
    auto nameLayout = new QHBoxLayout;
    mTitle += "раздела";

    // Виджеты для имени раздела
    auto nameLabel = new QLabel("Имя раздела: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(
        nameInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSectionDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    dlgItems.append(nameInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(nameLayout);
}
