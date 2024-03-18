#include "xmlalarmstatealldialog.h"

XmlAlarmStateAllDialog::XmlAlarmStateAllDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlAlarmStateAllDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto descLayout = new QHBoxLayout;
    auto addrLayout = new QHBoxLayout;
    auto typeLayout = new QHBoxLayout;
    mTitle += "сигнализации";

    // Виджеты для адреса сигнализации
    auto addrLabel = new QLabel("Адрес сигнализации: ", this);
    auto addrInput = new QSpinBox(this);
    addrInput->setMinimum(addrMin);
    addrInput->setMaximum(addrMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(addrInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlAlarmStateAllDialog::dataChanged));
#endif
    QObject::connect(addrInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlAlarmStateAllDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    dlgItems.append(addrInput);

    // Виджеты для описания сигнализации
    auto descLabel = new QLabel("Описание сигнализации: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlAlarmStateAllDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    dlgItems.append(descInput);

    // Виджеты для описания видимости поля данных
    auto typeLabel = new QLabel("Видимость поля на вкладке: ", this);
    auto typeInput = new QComboBox(this);
    typeInput->addItems({ "warning", "critical", "info" });
    typeInput->setCurrentIndex(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeInput);
    QObject::connect(                                                //
        typeInput, qOverload<int>(&QComboBox::currentIndexChanged),  //
        this, qOverload<int>(&XmlAlarmStateAllDialog::dataChanged)); //
    dlgItems.append(typeInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(descLayout);
    mainLayout->addLayout(typeLayout);
}
