#include "xml104dialog.h"

Xml104Dialog::Xml104Dialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void Xml104Dialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 180);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto Layout1 = new QHBoxLayout(this);
    auto Layout2 = new QHBoxLayout(this);
    auto Layout3 = new QHBoxLayout(this);
    auto Layout4 = new QHBoxLayout(this);

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    Layout1->addWidget(sigIdLabel);
    sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    Layout1->addWidget(sigIdInput);

    // Виджеты для типа регистра
    auto sigTypeLabel = new QLabel("Тип регистра: ", this);
    Layout2->addWidget(sigTypeLabel);
    sigTypeInput = new QLineEdit("", this);
    sigTypeInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    Layout2->addWidget(sigTypeInput);

    // Виджеты для типа возвращаемого значения
    auto transTypeLabel = new QLabel("Тип передачи: ", this);
    Layout3->addWidget(transTypeLabel);
    transTypeInput = new QLineEdit("", this);
    QObject::connect(
        transTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    Layout3->addWidget(transTypeInput);

    // Виджеты для описания
    auto sigGroupLabel = new QLabel("Группа сигналов: ", this);
    Layout4->addWidget(sigGroupLabel);
    sigGroupInput = new QLineEdit("", this);
    QObject::connect(
        sigGroupInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    Layout4->addWidget(sigGroupInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание элемента IEC60870");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 4);
        setWindowTitle("Редактирование элемента IEC60870");
        sigIdInput->setText(selectedData[0]);
        sigTypeInput->setText(selectedData[1]);
        transTypeInput->setText(selectedData[2]);
        sigGroupInput->setText(selectedData[3]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    mainLayout->addLayout(Layout3);
    mainLayout->addLayout(Layout4);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &Xml104Dialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList Xml104Dialog::collectData()
{
    return {
        sigIdInput->text(), sigTypeInput->text(),     //
        transTypeInput->text(), sigGroupInput->text() //
    };
}
