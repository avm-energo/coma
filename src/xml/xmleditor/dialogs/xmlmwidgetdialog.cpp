#include "xmlmwidgetdialog.h"

XmlMWidgetDialog::XmlMWidgetDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlMWidgetDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto Layout1 = new QHBoxLayout;
    auto Layout2 = new QHBoxLayout;

    // Виджеты для имени мульти-виджета
    auto descLabel = new QLabel("Имя: ", this);
    Layout1->addWidget(descLabel);
    desc = new QLineEdit("", this);
    QObject::connect(desc, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout1->addWidget(desc);

    // Виджеты для начального адреса
    auto addrLabel = new QLabel("Начальный адрес: ", this);
    Layout2->addWidget(addrLabel);
    startAddr = new QLineEdit("", this);
    startAddr->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        startAddr, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout2->addWidget(startAddr);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание описания мульти-виджета");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 2);
        setWindowTitle("Редактирование описания мульти-виджета");
        desc->setText(selectedData[0]);
        startAddr->setText(selectedData[1]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

QStringList XmlMWidgetDialog::collectData()
{
    return { desc->text(), startAddr->text() };
}
