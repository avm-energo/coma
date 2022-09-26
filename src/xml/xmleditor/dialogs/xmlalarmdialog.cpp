#include "xmlalarmdialog.h"

XmlAlarmDialog::XmlAlarmDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlAlarmDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto descLayout = new QHBoxLayout;
    auto addrLayout = new QHBoxLayout;
    mTitle += "сигнализации";

    // Виджеты для описания сигнализации
    auto descLabel = new QLabel("Описание сигнализации: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    dlgItems.append(descInput);

    // Виджеты для адреса сигнализации
    auto addrLabel = new QLabel("Адрес сигнализации: ", this);
    auto addrInput = new QLineEdit("", this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(addrInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    dlgItems.append(addrInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(descLayout);
    mainLayout->addLayout(addrLayout);
}
