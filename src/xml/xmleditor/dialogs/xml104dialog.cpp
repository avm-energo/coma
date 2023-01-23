#include "xml104dialog.h"

Xml104Dialog::Xml104Dialog(QWidget *parent) : XmlDialog(parent)
{
}

void Xml104Dialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 180);
    // Создание слоёв окна
    auto sigIdLayout = new QHBoxLayout;
    auto sigTypeLayout = new QHBoxLayout;
    auto transTypeLayout = new QHBoxLayout;
    auto sigGrpLayout = new QHBoxLayout;
    mTitle += "элемента IEC60870";

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    auto sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigIdLayout->addWidget(sigIdLabel);
    sigIdLayout->addWidget(sigIdInput);
    dlgItems.append(sigIdInput);

    // Виджеты для типа регистра
    auto sigTypeLabel = new QLabel("Тип регистра: ", this);
    auto sigTypeInput = new QLineEdit("", this);
    sigTypeInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigTypeLayout->addWidget(sigTypeLabel);
    sigTypeLayout->addWidget(sigTypeInput);
    dlgItems.append(sigTypeInput);

    // Виджеты для типа возвращаемого значения
    auto transTypeLabel = new QLabel("Тип передачи: ", this);
    auto transTypeInput = new QLineEdit("", this);
    QObject::connect(
        transTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    transTypeLayout->addWidget(transTypeLabel);
    transTypeLayout->addWidget(transTypeInput);
    dlgItems.append(transTypeInput);

    // Виджеты для описания
    auto sigGroupLabel = new QLabel("Группа сигналов: ", this);
    auto sigGroupInput = new QLineEdit("", this);
    QObject::connect(
        sigGroupInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigGrpLayout->addWidget(sigGroupLabel);
    sigGrpLayout->addWidget(sigGroupInput);
    dlgItems.append(sigGroupInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(sigIdLayout);
    mainLayout->addLayout(sigTypeLayout);
    mainLayout->addLayout(transTypeLayout);
    mainLayout->addLayout(sigGrpLayout);
}
