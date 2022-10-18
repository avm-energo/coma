#include "xmlmwidgetdialog.h"

#include "../models/xmlmodels.h"

XmlMWidgetDialog::XmlMWidgetDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlMWidgetDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 300);
    // Создание слоёв окна
    auto addrLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    auto countLayout = new QHBoxLayout;
    auto tooltipLayout = new QHBoxLayout;
    auto viewLayout = new QHBoxLayout;
    auto strArrLayout = new QHBoxLayout;
    mTitle += "описания мульти-виджета";

    // Виджеты для начального адреса
    auto addrLabel = new QLabel("Начальный адрес: ", this);
    auto startAddr = new QLineEdit("", this);
    startAddr->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        startAddr, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(startAddr);
    dlgItems.append(startAddr);

    // Виджеты для имени мульти-виджета
    auto descLabel = new QLabel("Имя: ", this);
    auto desc = new QLineEdit("", this);
    QObject::connect(desc, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(desc);
    dlgItems.append(desc);

    // Виджеты для количества адресов
    auto countLabel = new QLabel("Количество: ", this);
    auto count = new QLineEdit("", this);
    count->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(count, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    countLayout->addWidget(countLabel);
    countLayout->addWidget(count);
    dlgItems.append(count);

    // Виджеты для тултипа
    auto tooltipLabel = new QLabel("Тултип: ", this);
    auto tooltip = new QLineEdit("", this);
    QObject::connect(tooltip, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    tooltipLayout->addWidget(tooltipLabel);
    tooltipLayout->addWidget(tooltip);
    dlgItems.append(tooltip);

    // Виджеты для типа отображения
    auto viewLabel = new QLabel("Тип отображения: ", this);
    view = new QComboBox(this);
    view->addItems({ "float", "bitset" });
    view->setEditable(true);
    view->setCurrentIndex(0);
    viewLayout->addWidget(viewLabel);
    viewLayout->addWidget(view);
    QObject::connect(                                          //
        view, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlMWidgetDialog::dataChanged)); //

    // Виджеты для массива строк
    auto strArrLabel = new QLabel("String Array: ", this);
    auto strArray = new QLineEdit("", this);
    QObject::connect(
        strArray, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    strArrLayout->addWidget(strArrLabel);
    strArrLayout->addWidget(strArray);
    dlgItems.append(strArray);

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(descLayout);
    mainLayout->addLayout(countLayout);
    mainLayout->addLayout(tooltipLayout);
    mainLayout->addLayout(viewLayout);
    mainLayout->addLayout(strArrLayout);
}

QStringList XmlMWidgetDialog::collectData()
{
    auto retVal = XmlDialog::collectData();
    retVal.append(view->currentText());
    return retVal;
}

void XmlMWidgetDialog::modelDataResponse(const QStringList &response)
{
    XmlDialog::modelDataResponse(response);
    view->setCurrentText(response.last());
}
