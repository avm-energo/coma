#include "xmlprotocomdialog.h"

XmlProtocomDialog::XmlProtocomDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlProtocomDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto Layout1 = new QHBoxLayout(this);
    auto Layout2 = new QHBoxLayout(this);

    // Виджеты для описания
    auto blockLabel = new QLabel("Номер блока: ", this);
    Layout1->addWidget(blockLabel);
    blockInput = new QLineEdit("", this);
    blockInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        blockInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlProtocomDialog::dataChanged));
    Layout1->addWidget(blockInput);

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    Layout2->addWidget(sigIdLabel);
    sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlProtocomDialog::dataChanged));
    Layout2->addWidget(sigIdInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание элемента Protocom");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setWindowTitle("Редактирование элемента Protocom");
        blockInput->setText(selectedData[0]);
        sigIdInput->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlProtocomDialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList XmlProtocomDialog::collectData()
{
    return { blockInput->text(), sigIdInput->text() };
}
