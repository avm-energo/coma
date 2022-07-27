#include "xmlmwidgetdialog.h"

#include "../models/xmlsgroupmodel.h"

XmlMWidgetDialog::XmlMWidgetDialog(XmlSortProxyModel *model, QWidget *parent) : XmlDialog(model, parent)
{
}

void XmlMWidgetDialog::setupUI(QStringList &selectedData)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 300);
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    auto Layout1 = new QHBoxLayout;
    auto Layout2 = new QHBoxLayout;
    auto Layout3 = new QHBoxLayout;
    auto Layout4 = new QHBoxLayout;
    auto Layout5 = new QHBoxLayout;

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

    // Виджеты для количества адресов
    auto countLabel = new QLabel("Количество: ", this);
    Layout3->addWidget(countLabel);
    count = new QLineEdit("", this);
    count->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(count, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout3->addWidget(count);

    // Виджеты для тултипа
    auto tooltipLabel = new QLabel("Тултип: ", this);
    Layout4->addWidget(tooltipLabel);
    tooltip = new QLineEdit("", this);
    QObject::connect(tooltip, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout4->addWidget(tooltip);

    // Виджеты для
    auto strArrLabel = new QLabel("String Array: ", this);
    Layout5->addWidget(strArrLabel);
    strArray = new QLineEdit("", this);
    QObject::connect(
        strArray, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMWidgetDialog::dataChanged));
    Layout5->addWidget(strArray);

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
        auto qvarData = mModel->data(mModel->index(mRow, 0), SGroupDataRole);
        auto mWidgetData = qvarData.value<SGroupHideData>();
        count->setText(QString::number(mWidgetData.count));
        tooltip->setText(mWidgetData.tooltip);
        strArray->setText(accumulateStr(mWidgetData.array));
    }

    // Добавляем слои на главный слой
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    mainLayout->addLayout(Layout3);
    mainLayout->addLayout(Layout4);
    mainLayout->addLayout(Layout5);
    // Кнопка для сохранения изменений
    addSaveBtnAndApply(mainLayout);
}

void XmlMWidgetDialog::saveData()
{
    // Если есть изменения
    if (isChanged)
    {
        // Собираем изменения из полей
        auto collectedData = collectData();
        QStringList part1 = { collectedData[0], collectedData[1] };
        QStringList part2 = { collectedData[2], collectedData[3], collectedData[4] };
        // Записываем изменения в модель
        writeData(part1);
        mModel->setData(mModel->index(mRow, 0), QVariant::fromValue(convertHideData(part2)), SGroupDataRole);
        // Изменяем состояние (данные сохранены, следовательно не изменены)
        isChanged = false;
        // После сохранения изменений в модель меняем заголовок окна
        auto curTitle = windowTitle();
        curTitle = curTitle.left(curTitle.lastIndexOf(" [ИЗМЕНЕНО]"));
        setWindowTitle(curTitle);
    }
}

QStringList XmlMWidgetDialog::collectData()
{
    return {
        desc->text(), startAddr->text(), count->text(), //
        tooltip->text(), strArray->text()               //
    };
}
