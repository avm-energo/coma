#include "xmlconfigdialog.h"

XmlConfigDialog::XmlConfigDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlConfigDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 200);
    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto defLayout = new QHBoxLayout;
    auto countLayout = new QHBoxLayout;
    auto visibLayout = new QHBoxLayout;
    mTitle += "конфига";

    // Виджеты для ID конфига
    auto idLabel = new QLabel("S2 ID: ", this);
    auto idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    dlgItems.append(idInput);

    // Виджеты для значения по умолчанию конфига
    auto defLabel = new QLabel("Значение по умолчанию: ", this);
    auto defaultInput = new QLineEdit("", this);
    QObject::connect(
        defaultInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    defLayout->addWidget(defLabel);
    defLayout->addWidget(defaultInput);
    dlgItems.append(defaultInput);

    // Виджеты для уточнённого значения количества элементов конфига
    auto countLabel = new QLabel("Уточнённое кол-во: ", this);
    auto countInput = new QLineEdit("", this);
    countInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        countInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countInput);
    dlgItems.append(countInput);

    // Виджеты для видимости конфигурационного параметра
    auto visibLabel = new QLabel("Видимость: ", this);
    visibility = new QComboBox(this);
    visibility->addItems({ "true", "false" });
    visibility->setEditable(true);
    visibility->setCurrentIndex(0);
    visibLayout->addWidget(visibLabel);
    visibLayout->addWidget(visibility);
    QObject::connect(                                                //
        visibility, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlConfigDialog::dataChanged));        //

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(defLayout);
    mainLayout->addLayout(countLayout);
    mainLayout->addLayout(visibLayout);
}

QStringList XmlConfigDialog::collectData()
{
    auto retVal = XmlDialog::collectData();
    retVal.append(visibility->currentText());
    return retVal;
}

void XmlConfigDialog::modelDataResponse(const QStringList &response)
{
    XmlDialog::modelDataResponse(response);
    visibility->setCurrentText(response.last());
}
