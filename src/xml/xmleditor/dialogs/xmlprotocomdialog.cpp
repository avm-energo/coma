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
    auto blockLayout = new QHBoxLayout;
    auto sigIdLayout = new QHBoxLayout;

    // Виджеты для описания
    auto blockLabel = new QLabel("Номер блока: ", this);
    blockLayout->addWidget(blockLabel);
    blockInput = new QLineEdit("", this);
    blockInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        blockInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlProtocomDialog::dataChanged));
    blockLayout->addWidget(blockInput);

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    sigIdLayout->addWidget(sigIdLabel);
    sigIdInput = new QLineEdit("", this);
    sigIdInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigIdInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlProtocomDialog::dataChanged));
    sigIdLayout->addWidget(sigIdInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setTitle("Создание элемента Protocom");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setTitle("Редактирование элемента Protocom");
        blockInput->setText(selectedData[0]);
        sigIdInput->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(blockLayout);
    mainLayout->addLayout(sigIdLayout);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList XmlProtocomDialog::collectData()
{
    return { blockInput->text(), sigIdInput->text() };
}
