#include "xmlsgroupdialog.h"

XmlSGroupDialog::XmlSGroupDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlSGroupDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto Layout1 = new QHBoxLayout;
    auto Layout2 = new QHBoxLayout;

    // Виджеты для имени группы
    auto nameLabel = new QLabel("Имя группы: ", this);
    Layout1->addWidget(nameLabel);
    nameInput = new QLineEdit("", this);
    QObject::connect(
        nameInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSGroupDialog::dataChanged));
    Layout1->addWidget(nameInput);

    // Виджеты для ID вкладки
    auto idLabel = new QLabel("ID вкладки: ", this);
    Layout2->addWidget(idLabel);
    idTab = new QLineEdit("", this);
    idTab->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(idTab, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSGroupDialog::dataChanged));
    Layout2->addWidget(idTab);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание группы");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setWindowTitle("Редактирование группы");
        nameInput->setText(selectedData[0]);
        idTab->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList XmlSGroupDialog::collectData()
{
    return { nameInput->text(), idTab->text() };
}
