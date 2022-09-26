#include "xmlconfigdialog.h"

XmlConfigDialog::XmlConfigDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlConfigDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto defLayout = new QHBoxLayout;
    mTitle += "элемента Protocom";

    // Виджеты для описания
    auto idLabel = new QLabel("S2 ID: ", this);
    auto idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    dlgItems.append(idInput);

    // Виджеты для ID сигнала
    auto defLabel = new QLabel("Значение по умолчанию: ", this);
    auto defaultInput = new QLineEdit("", this);
    QObject::connect(
        defaultInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    defLayout->addWidget(defLabel);
    defLayout->addWidget(defaultInput);
    dlgItems.append(defaultInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(defLayout);
}
