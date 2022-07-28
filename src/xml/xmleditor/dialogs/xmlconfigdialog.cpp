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
    auto idLayout = new QHBoxLayout;
    auto defLayout = new QHBoxLayout;

    // Виджеты для описания
    auto idLabel = new QLabel("S2 ID: ", this);
    idLayout->addWidget(idLabel);
    idInput = new QLineEdit("", this);
    idInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    idLayout->addWidget(idInput);

    // Виджеты для ID сигнала
    auto defLabel = new QLabel("Значение по умолчанию: ", this);
    defLayout->addWidget(defLabel);
    defaultInput = new QLineEdit("", this);
    QObject::connect(
        defaultInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlConfigDialog::dataChanged));
    defLayout->addWidget(defaultInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setTitle("Создание элемента Protocom");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setTitle("Редактирование элемента Protocom");
        idInput->setText(selectedData[0]);
        defaultInput->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(idLayout);
    mainLayout->addLayout(defLayout);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList XmlConfigDialog::collectData()
{
    return { idInput->text(), defaultInput->text() };
}
