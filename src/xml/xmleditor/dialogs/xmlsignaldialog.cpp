#include "xmlsignaldialog.h"

XmlSignalDialog::XmlSignalDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlSignalDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 160);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto idLayout = new QHBoxLayout;
    auto addrLayout = new QHBoxLayout;
    auto countLayout = new QHBoxLayout;

    // Виджеты для ID группы сигналов
    auto idLabel = new QLabel("ID группы: ", this);
    idLayout->addWidget(idLabel);
    idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    idLayout->addWidget(idInput);

    // Виджеты для адресов группы сигналов
    auto addrLabel = new QLabel("Адрес начального сигнала: ", this);
    addrLayout->addWidget(addrLabel);
    addrInput = new QLineEdit("", this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        addrInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    addrLayout->addWidget(addrInput);

    // Виджеты для количества сигналов в группе
    auto countLabel = new QLabel("Количество сигналов в группе: ", this);
    countLayout->addWidget(countLabel);
    countInput = new QLineEdit("", this);
    countInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        countInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSignalDialog::dataChanged));
    countLayout->addWidget(countInput);

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

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(addrLayout);
    mainLayout->addLayout(countLayout);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList XmlSignalDialog::collectData()
{
    return { idInput->text(), addrInput->text(), countInput->text() };
}
