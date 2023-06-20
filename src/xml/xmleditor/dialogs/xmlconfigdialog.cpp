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
    auto idInput = new QSpinBox(this);
    idInput->setMinimum(idMin);
    idInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(idInput, &QSpinBox::textChanged, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
#endif
    QObject::connect(idInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlConfigDialog::dataChanged));
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
    auto countInput = new QSpinBox(this);
    countInput->setMinimum(configCountMin);
    countInput->setMaximum(configCountMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(countInput, &QSpinBox::textChanged, //
        this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
#endif
    QObject::connect(countInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlConfigDialog::dataChanged));
    countLayout->addWidget(countLabel);
    countLayout->addWidget(countInput);
    dlgItems.append(countInput);

    // Виджеты для видимости конфигурационного параметра
    auto visibLabel = new QLabel("Видимость: ", this);
    auto visibInput = new QComboBox(this);
    visibInput->addItems({ "true", "false" });
    visibInput->setCurrentIndex(0);
    visibLayout->addWidget(visibLabel);
    visibLayout->addWidget(visibInput);
    QObject::connect(                                                //
        visibInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlConfigDialog::dataChanged));        //
    dlgItems.append(visibInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(defLayout);
    mainLayout->addLayout(countLayout);
    mainLayout->addLayout(visibLayout);
}
