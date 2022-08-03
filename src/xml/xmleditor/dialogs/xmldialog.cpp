#include "xmldialog.h"

#include "../../../widgets/epopup.h"

#include <QGuiApplication>
#include <QLineEdit>
#include <QScreen>

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
        auto input = qobject_cast<QLineEdit *>(item);
        collected.append(input->text());
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

bool XmlDialog::checkDataBeforeSaving()
{
    return true;
}

void XmlDialog::modelDataResponse(const QStringList &response)
{
    auto sizeR = response.count(), sizeD = dlgItems.count();
    auto size = (sizeR > sizeD ? sizeD : sizeR);
    for (auto i = 0; i < size; i++)
    {
        auto input = qobject_cast<QLineEdit *>(dlgItems[i]);
        input->setText(response[i]);
    }
}

void XmlDialog::saveData()
{
    // Если есть изменения
    if (isChanged)
    {
        if (checkDataBeforeSaving())
        {
            // Собираем изменения из полей
            auto collectedData = collectData();
            // Отсылаем изменения в модель
            if (mRow == createId)
                emit createData(collectedData, &mRow);
            else
                emit updateData(collectedData, mRow);
            // После сохранения изменений в модель меняем заголовок окна
            setWindowTitle(mTitle);
            // Изменяем состояние (данные сохранены, следовательно не изменены)
            isChanged = false;
        }
        else
            EMessageBox::warning(this, "");
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
