#include "xmldialog.h"

#include "../../../widgets/epopup.h"

#include <QGuiApplication>
#include <QScreen>
#include <gen/std_ext.h>

XmlDialog::XmlDialog(QWidget *parent) : QDialog(parent, Qt::Window), isChanged(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void XmlDialog::startup(int row)
{
    mRow = row; ///< row = -1, если диалог создаётся для создания item-а
    if (mRow == createId)
    {
        mTitle = "Создание ";
        isChanged = true;
    }
    else
        mTitle = "Редактирование ";

    auto mainLayout = new QVBoxLayout(this);
    setupUI(mainLayout);
    setWindowTitle(mTitle);
    addSaveBtnAndApply(mainLayout);
    setLayout(mainLayout);
    emit modelDataRequest(row);
}

void XmlDialog::setupSizePos(int width, int height)
{
    // Настройки окна (размер, положение)
    auto center = QGuiApplication::primaryScreen()->geometry().center();
    setGeometry(center.x() - width / 2, center.y() - height / 2, width, height);
    setFixedSize(this->size());
}

void XmlDialog::addSaveBtnAndApply(QVBoxLayout *main)
{
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlDialog::saveData);
    main->addWidget(saveButton);
}

QStringList XmlDialog::collectData()
{
    QStringList collected;
    for (auto &item : dlgItems)
    {
        std::visit( //
            overloaded {
                [&](const QLineEdit *widget) { //
                    collected.append(widget->text());
                },
                [&](const QComboBox *widget) { //
                    collected.append(widget->currentText());
                },
                [&](const QSpinBox *widget) { //
                    auto value = QString::number(widget->value());
                    collected.append(value);
                },
            },
            item);
    }
    return collected;
}

void XmlDialog::reject()
{
    // Если есть изменения, то показываем пользователю MessageBox
    if (isChanged)
    {
        if (EMessageBox::question("Сохранить изменения?"))
            saveData();
    }
    QDialog::reject();
}

bool XmlDialog::checkDataBeforeSaving(const QStringList &savedData)
{
    // TODO: метод должен быть pure virtual
    Q_UNUSED(savedData);
    return true;
}

void XmlDialog::modelDataResponse(const QStringList &response)
{
    auto responseCount = response.count(), widgetsCount = dlgItems.count();
    // Находим минимальное количество, при несовпадении игнорируем
    auto size = (responseCount > widgetsCount ? widgetsCount : responseCount);
    for (auto i = 0; i < size; i++)
    {
        auto &item = dlgItems[i];
        std::visit( //
            overloaded {
                [&](QLineEdit *widget) { //
                    widget->setText(response[i]);
                },
                [&](QComboBox *widget) { //
                    auto itemIndex = widget->findText(response[i]);
                    if (itemIndex != -1)
                        widget->setCurrentIndex(itemIndex);
                    else
                        widget->setCurrentText(response[i]);
                },
                [&](QSpinBox *widget) { //
                    widget->setValue(response[i].toInt());
                },
            },
            item);
    }
    resetChangeState();
}

void XmlDialog::saveData()
{
    // Если есть изменения
    if (isChanged)
    {
        // Собираем изменения из полей
        auto collectedData = collectData();
        // Проверяем данные перед сохранением
        if (checkDataBeforeSaving(collectedData))
        {
            // Отсылаем изменения в модель
            if (mRow == createId)
                emit createData(collectedData, &mRow);
            else
                emit updateData(collectedData, mRow);
            // После сохранения изменений в модель меняем заголовок окна
            setWindowTitle(mTitle);
            // Изменяем состояние (данные сохранены, следовательно не изменены)
            isChanged = false;
            this->reject();
        }
        // Если данные окна не прошли проверку
        else
            EMessageBox::warning(this, "Введены некорректные данные");
    }
}

void XmlDialog::dataChanged()
{
    // Если появились изменения, меняем заголовок окна
    if (!isChanged)
    {
        setWindowTitle(mTitle + " [ИЗМЕНЕНО]");
        // Изменяем состояние (данные изменены)
        isChanged = true;
    }
}

void XmlDialog::dataChanged(const QString &strData)
{
    Q_UNUSED(strData);
    dataChanged();
}

void XmlDialog::dataChanged(int index)
{
    Q_UNUSED(index);
    dataChanged();
}

void XmlDialog::resetChangeState()
{
    // Сброс изменённого состояния окна
    if (isChanged)
    {
        setWindowTitle(mTitle);
        isChanged = false;
    }
}
