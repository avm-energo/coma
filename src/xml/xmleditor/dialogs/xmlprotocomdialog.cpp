#include "xmlprotocomdialog.h"

XmlProtocomDialog::XmlProtocomDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlProtocomDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto blockLayout = new QHBoxLayout;
    auto sigIdLayout = new QHBoxLayout;
    mTitle += "элемента Protocom";

    // Виджеты для описания
    auto blockLabel = new QLabel("Номер блока: ", this);
    auto blockInput = new QLineEdit("", this);
    blockInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        blockInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlProtocomDialog::dataChanged));
    blockLayout->addWidget(blockLabel);
    blockLayout->addWidget(blockInput);
    dlgItems.append(blockInput);

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    auto sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlProtocomDialog::dataChanged));
    sigIdLayout->addWidget(sigIdLabel);
    sigIdLayout->addWidget(sigIdInput);
    dlgItems.append(sigIdInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(blockLayout);
    mainLayout->addLayout(sigIdLayout);
}
