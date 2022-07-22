#include "xmlmeasjourdialog.h"

XmlMeasJourDialog::XmlMeasJourDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlMeasJourDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 80);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto headLayout = new QHBoxLayout(this);

    // Виджеты для описания события
    auto headLabel = new QLabel("Заголовок события: ", this);
    headLayout->addWidget(headLabel);
    headInput = new QLineEdit("", this);
    QObject::connect(
        headInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMeasJourDialog::dataChanged));
    headLayout->addWidget(headInput);

    // Окно для создания item-а
    if (selectedData.isEmpty())
        setWindowTitle("Создание события журнала измерений");
    // Окно для редактирования item-а
    else
    {
        Q_ASSERT(selectedData.size() == 1);
        setWindowTitle("Редактирование события журнала измерений");
        headInput->setText(selectedData[0]);
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(headLayout);
    // Кнопка для сохранения изменений
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlMeasJourDialog::saveData);
    mainLayout->addWidget(saveButton);
    setLayout(mainLayout);
}

QStringList XmlMeasJourDialog::collectData()
{
    return { headInput->text() };
}
