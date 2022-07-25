#include "xmldialog.h"

#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>

XmlDialog::XmlDialog(XmlSortProxyModel *model, QWidget *parent)
    : QDialog(parent, Qt::Window), mModel(model), isChanged(false), mRow(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

/// \brief Функция для вызова виртуального метода setupUI
/// \details Вызывает setupUI, в качестве параметров
/// передаётся строка с данными из модели
void XmlDialog::setupUICall(int &row)
{
    mRow = row; ///< row = -1, если диалог создаётся для создания item-а
    auto selectedData = getSelectedData();
    setupUI(selectedData);
    this->exec();
}

/// \brief Функция, вызываемая при закрытии диалогового окна
void XmlDialog::reject()
{
    // Если есть изменения, то показываем пользователю MessageBox
    if (isChanged)
    {
        auto resBtn = QMessageBox::question(this, "Сохранение", tr("Сохранить изменения?\n"),
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
            saveData();
    }
    QDialog::reject();
}

/// \brief Функция возвращает список с данными указанной строки из модели
QStringList XmlDialog::getSelectedData()
{
    // Если row = -1, то возвращаем пустой список (item создаётся с нуля)
    if (mRow == createId)
    {
        isChanged = true;
        return {};
    }
    else
    {
        QStringList selectedData;
        auto cols = mModel->columnCount();
        for (auto i = 0; i < cols; i++)
        {
            auto index = mModel->index(mRow, i);
            selectedData.append(mModel->data(index).value<QString>());
        }
        return selectedData;
    }
}

/// \brief Функция устанавливает фиксированный размер окна и отображает его в центре экрана
void XmlDialog::setupSizePos(int width, int height)
{
    // Настройки окна (размер, положение)
    auto center = QGuiApplication::primaryScreen()->geometry().center();
    setGeometry(center.x() - width / 2, center.y() - height / 2, width, height);
    setFixedSize(this->size());
}

/// \brief Функция принимает главный слой, добавляет туда кнопку
/// для сохранения данных и устанавливает слой в качестве основного
void XmlDialog::addSaveBtnAndApply(QVBoxLayout *main)
{
    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlDialog::saveData);
    main->addWidget(saveButton);
    setLayout(main);
}

/// \brief Функция для записи списка с данными в модель по указанной строке
void XmlDialog::writeData(QStringList &saved)
{
    auto cols = mModel->columnCount();
    if (cols == saved.size())
    {
        // Создаём данные
        if (mRow == createId)
        {
            ;
            ;
        }
        // Редактируем данные
        else
        {
            for (auto i = 0; i < cols; i++)
            {
                auto index = mModel->index(mRow, i);
                mModel->setData(index, saved[i]);
            }
        }
    }
}

/// \brief Слот, вызываемый при изменении данных в диалоговом окне
void XmlDialog::dataChanged()
{
    // Если появились изменения, меняем заголовок окна
    if (!isChanged)
        setWindowTitle(windowTitle() + " [ИЗМЕНЕНО]");
    // Изменяем состояние (данные изменены)
    isChanged = true;
}

void XmlDialog::saveData()
{
    // Если есть изменения
    if (isChanged)
    {
        // Собираем изменения из полей
        auto collectedData = collectData();
        // Записываем изменения в модель
        writeData(collectedData);
        // Изменяем состояние (данные сохранены, следовательно не изменены)
        isChanged = false;
        // После сохранения изменений в модель меняем заголовок окна
        auto curTitle = windowTitle();
        curTitle = curTitle.left(curTitle.lastIndexOf(" [ИЗМЕНЕНО]"));
        setWindowTitle(curTitle);
    }
}

/// \brief Перегрузка слота, вызываемого при изменении данных в диалоговом окне
/// \see dataChanged
void XmlDialog::dataChanged(const QString &strData)
{
    Q_UNUSED(strData);
    dataChanged();
}
