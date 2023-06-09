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
    auto hlLayout = new QHBoxLayout;
    mTitle += "сигнализации";

    // Виджеты для адреса сигнализации
    auto addrLabel = new QLabel("Адрес сигнализации: ", this);
    auto addrInput = new QSpinBox(this);
    addrInput->setMinimum(addrMin);
    addrInput->setMaximum(addrMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(addrInput, &QSpinBox::textChanged, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
#endif
    QObject::connect(addrInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlAlarmDialog::dataChanged));
    addrLayout->addWidget(addrLabel);
    addrLayout->addWidget(addrInput);
    dlgItems.append(addrInput);

    // Виджеты для описания сигнализации
    auto descLabel = new QLabel("Описание сигнализации: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    dlgItems.append(descInput);

    // Виджеты для подсветки сигналов (hl - highlights, not half-life!)
    auto hlLabel = new QLabel("Подсветка: ", this);
    auto hlInput = new QLineEdit("", this);
    QObject::connect(hlInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
    hlLayout->addWidget(hlLabel);
    hlLayout->addWidget(hlInput);
    dlgItems.append(hlInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(descLayout);
    mainLayout->addLayout(hlLayout);
}
