#include "xmlworkjourdialog.h"

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
    mTitle += "события рабочего журнала";

    // Виджеты для адреса события
    auto addrLabel = new QLabel("Адрес события: ", this);
    auto addrInput = new QLineEdit("", this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        addrInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlWorkJourDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    dlgItems.append(addrInput);

    // Виджеты для описания события
    auto descLabel = new QLabel("Описание: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(
        descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlWorkJourDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    dlgItems.append(descInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(descLayout);
}
