#include "xmlsignaldialog.h"

XmlSignalDialog::XmlSignalDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlSignalDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 160);
    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto addrLayout = new QHBoxLayout;
    auto countLayout = new QHBoxLayout;
    mTitle += "группы сигналов";

    // Виджеты для ID группы сигналов
    auto idLabel = new QLabel("ID группы: ", this);
    auto idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    dlgItems.append(idInput);

    // Виджеты для адресов группы сигналов
    auto addrLabel = new QLabel("Адрес начального сигнала: ", this);
    auto addrInput = new QLineEdit("", this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        addrInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    dlgItems.append(addrInput);

    // Виджеты для количества сигналов в группе
    auto countLabel = new QLabel("Количество сигналов в группе: ", this);
    auto countInput = new QLineEdit("", this);
    countInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        countInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countInput);
    dlgItems.append(countInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(countLayout);
}
