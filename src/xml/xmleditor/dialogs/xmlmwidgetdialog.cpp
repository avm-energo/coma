#include "xmlmwidgetdialog.h"

#include "../models/xmlsgroupmodel.h"

XmlMWidgetDialog::XmlMWidgetDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlMWidgetDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 300);
    // Создание слоёв окна
    auto Layout1 = new QHBoxLayout;
    auto Layout2 = new QHBoxLayout;
    auto Layout3 = new QHBoxLayout;
    auto Layout4 = new QHBoxLayout;
    auto Layout5 = new QHBoxLayout;
    mTitle += "описания мульти-виджета";

    // Виджеты для имени мульти-виджета
    auto descLabel = new QLabel("Имя: ", this);
    auto desc = new QLineEdit("", this);
    QObject::connect(desc, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout1->addWidget(descLabel);
    Layout1->addWidget(desc);
    dlgItems.append(desc);

    // Виджеты для начального адреса
    auto addrLabel = new QLabel("Начальный адрес: ", this);
    auto startAddr = new QLineEdit("", this);
    startAddr->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        startAddr, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout2->addWidget(addrLabel);
    Layout2->addWidget(startAddr);
    dlgItems.append(startAddr);

    // Виджеты для количества адресов
    auto countLabel = new QLabel("Количество: ", this);
    auto count = new QLineEdit("", this);
    count->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(count, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout3->addWidget(countLabel);
    Layout3->addWidget(count);
    dlgItems.append(count);

    // Виджеты для тултипа
    auto tooltipLabel = new QLabel("Тултип: ", this);
    auto tooltip = new QLineEdit("", this);
    QObject::connect(tooltip, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout4->addWidget(tooltipLabel);
    Layout4->addWidget(tooltip);
    dlgItems.append(tooltip);

    // Виджеты для
    auto strArrLabel = new QLabel("String Array: ", this);
    auto strArray = new QLineEdit("", this);
    QObject::connect(
        strArray, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout5->addWidget(strArrLabel);
    Layout5->addWidget(strArray);
    dlgItems.append(strArray);

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    mainLayout->addLayout(Layout3);
    mainLayout->addLayout(Layout4);
    mainLayout->addLayout(Layout5);
}
