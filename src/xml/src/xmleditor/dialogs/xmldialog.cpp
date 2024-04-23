#include "xml/xmleditor/dialogs/xmldialog.h"

#include <QGuiApplication>
#include <QScreen>
#include <gen/std_ext.h>
#include <widgets/epopup.h>

XmlDialog::XmlDialog(QWidget *parent) : QDialog(parent, Qt::Window), m_isChanged(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void XmlDialog::startup(int row, BaseEditorModel *model)
{
    m_row = row; ///< row = -1, если диалог создаётся для создания item-а
    if (m_row == createId)
    {
        m_title = "Создание ";
        m_isChanged = true;
    }
    else
        m_title = "Редактирование ";

    auto mainLayout = new QVBoxLayout(this);
    setupUI(mainLayout);
    setWindowTitle(m_title);
    addSaveBtnAndApply(mainLayout);
    setLayout(mainLayout);
    if (m_row != createId)
        loadModelData(model->getRowData(row));
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
    for (auto &item : m_dlgItems)
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
    if (m_isChanged)
    {
        if (EMessageBox::question(this, "Сохранить изменения?"))
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

void XmlDialog::loadModelData(const QStringList &response)
{
    // Находим минимальное количество, при несовпадении игнорируем
    auto size = std::min(response.count(), m_dlgItems.count());
    for (auto i = 0; i < size; i++)
    {
        auto &item = m_dlgItems[i];
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
    if (m_isChanged)
    {
        // Собираем изменения из полей
        auto collectedData = collectData();
        // Проверяем данные перед сохранением
        if (checkDataBeforeSaving(collectedData))
        {
            // Отсылаем изменения в модель
            if (m_row == createId)
                emit createData(collectedData, &m_row);
            else
                emit updateData(collectedData, m_row);
            // После сохранения изменений в модель меняем заголовок окна
            setWindowTitle(m_title);
            // Изменяем состояние (данные сохранены, следовательно не изменены)
            m_isChanged = false;
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
    if (!m_isChanged)
    {
        setWindowTitle(m_title + " [ИЗМЕНЕНО]");
        // Изменяем состояние (данные изменены)
        m_isChanged = true;
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
    if (m_isChanged)
    {
        setWindowTitle(m_title);
        m_isChanged = false;
    }
}
