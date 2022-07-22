#include "xmlstabdialog.h"

XmlSTabDialog::XmlSTabDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlSTabDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 120);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto idLayout = new QHBoxLayout(this);
    auto nameLayout = new QHBoxLayout(this);

    // Виджеты для ID вкладки
    auto idLabel = new QLabel("ID вкладки: ", this);
    idLayout->addWidget(idLabel);
    idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSTabDialog::dataChanged));
    idLayout->addWidget(idInput);

    // Виджеты для имени вкладки
    auto nameLabel = new QLabel("Имя вкладки: ", this);
    nameLayout->addWidget(nameLabel);
    nameInput = new QLineEdit("", this);
    QObject::connect(nameInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSTabDialog::dataChanged));
    nameLayout->addWidget(nameInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание вкладки");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setWindowTitle("Редактирование вкладки");
        idInput->setText(selectedData[0]);
        nameInput->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(nameLayout);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlSTabDialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList XmlSTabDialog::collectData()
{
    return { idInput->text(), nameInput->text() };
}
