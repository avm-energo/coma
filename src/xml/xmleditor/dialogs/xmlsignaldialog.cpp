#include "xmlsignaldialog.h"

#include <QComboBox>

XmlSignalDialog::XmlSignalDialog(QWidget *parent) : XmlDialog(parent), type(nullptr)
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
    auto typeLayout = new QHBoxLayout;
    mTitle += "группы сигналов";

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

    // Виджеты для ID группы сигналов
    auto idLabel = new QLabel("ID группы: ", this);
    auto idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    dlgItems.append(idInput);

    // Виджеты для типа возвращаемого значения
    auto typeLabel = new QLabel("Тип возвращаемого значения: ", this);
    type = new QComboBox(this);
    type->addItems({ "Float", "BitString", "SinglePoint" });
    type->setEditable(true);
    type->setCurrentIndex(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(type);
    QObject::connect(                                          //
        type, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlSignalDialog::dataChanged));  //

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(countLayout);
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(typeLayout);
}

QStringList XmlSignalDialog::collectData()
{
    auto retVal = XmlDialog::collectData();
    retVal.append(type->currentText());
    return retVal;
}

void XmlSignalDialog::modelDataResponse(const QStringList &response)
{
    XmlDialog::modelDataResponse(response);
    type->setCurrentText(response.last());
    // type->setEditable(false);
}
