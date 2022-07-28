#include "xmlsectiondialog.h"

XmlSectionDialog::XmlSectionDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlSectionDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 80);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto Layout1 = new QHBoxLayout;

    // Виджеты для имени раздела
    auto nameLabel = new QLabel("Имя раздела: ", this);
    Layout1->addWidget(nameLabel);
    nameInput = new QLineEdit("", this);
    QObject::connect(
        nameInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlSectionDialog::dataChanged));
    Layout1->addWidget(nameInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setTitle("Создание раздела");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 1);
        setTitle("Редактирование раздела");
        nameInput->setText(selectedData[0]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList XmlSectionDialog::collectData()
{
    return { nameInput->text() };
}
