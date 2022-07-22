#include "xmlsignaldialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegExpValidator>

XmlSignalDialog::XmlSignalDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlSignalDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(600, 160);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto layout1 = new QHBoxLayout(this);
    auto layout2 = new QHBoxLayout(this);
    auto layout3 = new QHBoxLayout(this);

    // Виджеты для ID группы сигналов
    auto idLabel = new QLabel("ID группы: ", this);
    layout1->addWidget(idLabel);
    idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    layout1->addWidget(idInput);

    // Виджеты для адресов группы сигналов
    auto addrLabel = new QLabel("Адрес начального сигнала: ", this);
    layout2->addWidget(addrLabel);
    addrInput = new QLineEdit("", this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        addrInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    layout2->addWidget(addrInput);

    // Виджеты для количества сигналов в группе
    auto countLabel = new QLabel("Количество сигналов в группе: ", this);
    layout3->addWidget(countLabel);
    countInput = new QLineEdit("", this);
    countInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        countInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    layout3->addWidget(countInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание группы сигналов");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 3);
        setWindowTitle("Редактирование группы сигналов");
        idInput->setText(selectedData[0]);
        addrInput->setText(selectedData[1]);
        countInput->setText(selectedData[2]);
    }

    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    mainLayout->addLayout(layout3);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlSignalDialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList XmlSignalDialog::collectData()
{
    return { idInput->text(), addrInput->text(), countInput->text() };
}
