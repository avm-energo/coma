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
    auto sigIdLayout = new QHBoxLayout;
    auto sigTypeLayout = new QHBoxLayout;
    auto transTypeLayout = new QHBoxLayout;
    auto sigGrpLayout = new QHBoxLayout;

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    sigIdLayout->addWidget(sigIdLabel);
    sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigIdLayout->addWidget(sigIdInput);

    // Виджеты для типа регистра
    auto sigTypeLabel = new QLabel("Тип регистра: ", this);
    sigTypeLayout->addWidget(sigTypeLabel);
    sigTypeInput = new QLineEdit("", this);
    sigTypeInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigTypeLayout->addWidget(sigTypeInput);

    // Виджеты для типа возвращаемого значения
    auto transTypeLabel = new QLabel("Тип передачи: ", this);
    transTypeLayout->addWidget(transTypeLabel);
    transTypeInput = new QLineEdit("", this);
    QObject::connect(
        transTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    transTypeLayout->addWidget(transTypeInput);

    // Виджеты для описания
    auto sigGroupLabel = new QLabel("Группа сигналов: ", this);
    sigGrpLayout->addWidget(sigGroupLabel);
    sigGroupInput = new QLineEdit("", this);
    QObject::connect(
        sigGroupInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigGrpLayout->addWidget(sigGroupInput);

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
    mainLayout->addLayout(sigIdLayout);
    mainLayout->addLayout(sigTypeLayout);
    mainLayout->addLayout(transTypeLayout);
    mainLayout->addLayout(sigGrpLayout);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList Xml104Dialog::collectData()
{
    return {
        sigIdInput->text(), sigTypeInput->text(),     //
        transTypeInput->text(), sigGroupInput->text() //
    };
}
