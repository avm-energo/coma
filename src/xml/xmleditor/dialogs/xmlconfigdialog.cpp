#include "xmlconfigdialog.h"

XmlConfigDialog::XmlConfigDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlConfigDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto Layout1 = new QHBoxLayout(this);
    auto Layout2 = new QHBoxLayout(this);

    // Виджеты для описания
    auto idLabel = new QLabel("S2 ID: ", this);
    Layout1->addWidget(idLabel);
    idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    Layout1->addWidget(idInput);

    // Виджеты для ID сигнала
    auto defLabel = new QLabel("Значение по умолчанию: ", this);
    Layout2->addWidget(defLabel);
    defaultInput = new QLineEdit("", this);
    QObject::connect(
        defaultInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    Layout2->addWidget(defaultInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание элемента Protocom");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setWindowTitle("Редактирование элемента Protocom");
        idInput->setText(selectedData[0]);
        defaultInput->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlConfigDialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList XmlConfigDialog::collectData()
{
    return { idInput->text(), defaultInput->text() };
}
