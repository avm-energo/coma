#include "xmlalarmdialog.h"

XmlAlarmDialog::XmlAlarmDialog(XmlSortProxyModel *model, QWidget *parent)
    : XmlDialog(model, parent), descInput(nullptr), addrInput(nullptr)
{
}

void XmlAlarmDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto descLayout = new QHBoxLayout;
    auto addrLayout = new QHBoxLayout;

    // Виджеты для описания сигнализации
    auto descLabel = new QLabel("Описание сигнализации: ", this);
    descLayout->addWidget(descLabel);
    descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
    descLayout->addWidget(descInput);

    // Виджеты для адреса сигнализации
    auto addrLabel = new QLabel("Адрес сигнализации: ", this);
    addrLayout->addWidget(addrLabel);
    addrInput = new QLineEdit("", this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(addrInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
    addrLayout->addWidget(addrInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание сигнализации");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setWindowTitle("Редактирование сигнализации");
        descInput->setText(selectedData[0]);
        addrInput->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(descLayout);
    mainLayout->addLayout(addrLayout);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList XmlAlarmDialog::collectData()
{
    return { descInput->text(), addrInput->text() };
}
