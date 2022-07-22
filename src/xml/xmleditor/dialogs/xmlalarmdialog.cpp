#include "xmlalarmdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegExpValidator>

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
    auto layout1 = new QHBoxLayout(this);
    auto layout2 = new QHBoxLayout(this);

    // Виджеты для описания сигнализации
    auto descLabel = new QLabel("Описание сигнализации: ", this);
    layout1->addWidget(descLabel);
    descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
    layout1->addWidget(descInput);

    // Виджеты для адреса сигнализации
    auto addrLabel = new QLabel("Адрес сигнализации: ", this);
    layout2->addWidget(addrLabel);
    addrInput = new QLineEdit("", this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(addrInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlAlarmDialog::dataChanged));
    layout2->addWidget(addrInput);

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
    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlAlarmDialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList XmlAlarmDialog::collectData()
{
    return { descInput->text(), addrInput->text() };
}
