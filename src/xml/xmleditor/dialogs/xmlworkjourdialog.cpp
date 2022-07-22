#include "xmlworkjourdialog.h"

XmlWorkJourDialog::XmlWorkJourDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlWorkJourDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 120);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto addrLayout = new QHBoxLayout(this);
    auto descLayout = new QHBoxLayout(this);

    // Виджеты для адреса события
    auto addrLabel = new QLabel("Адрес события: ", this);
    addrLayout->addWidget(addrLabel);
    addrInput = new QLineEdit("", this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        addrInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlWorkJourDialog::dataChanged));
    addrLayout->addWidget(addrInput);

    // Виджеты для описания события
    auto descLabel = new QLabel("Описание: ", this);
    descLayout->addWidget(descLabel);
    descInput = new QLineEdit("", this);
    QObject::connect(
        descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlWorkJourDialog::dataChanged));
    descLayout->addWidget(descInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание события рабочего журнала");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setWindowTitle("Редактирование события рабочего журнала");
        addrInput->setText(selectedData[0]);
        descInput->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(descLayout);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlWorkJourDialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList XmlWorkJourDialog::collectData()
{
    return { addrInput->text(), descInput->text() };
}
