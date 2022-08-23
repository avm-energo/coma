#include "datacontroller.h"

#include "../../gen/stdfunc.h"
#include "../../widgets/epopup.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

DataController::DataController(QObject *parent) : QObject(parent), isModuleChanged(false), changedRow(-1)
{
}

/// \brief Возвращает путь до файла в домашней директории пользователя.
QString DataController::getFilePath(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    return dir.filePath(filename);
}

/// \brief Установка флага, что файл не изменён.
void DataController::resetOrSaved()
{
    isModuleChanged = false;
}

/// \brief Возвращает флаг, изменён файл или нет.
bool DataController::getModuleState() const
{
    return isModuleChanged;
}

/// \brief Сохраняет номер строки изменённого конфига в мастер модели.
int DataController::getRow() const
{
    return changedRow;
}

/// \brief Возвращает сохранённую строку изменённого конфига.
void DataController::setRow(const int &row)
{
    changedRow = row;
}

/// \brief Изменяет состояние флага, изменён ли файл, и отправляет
/// сообщение редактору, чтобы выделить изменённый элемент.
void DataController::configChanged()
{
    if (!isModuleChanged)
    {
        isModuleChanged = true;
        emit highlightModified();
    }
}

/// \brief Создать файл (создание конфига модуля).
void DataController::createFile(const QStringList &creationData)
{
    auto newFilePath = getFilePath(creationData.last());
    auto templateFilePath = getFilePath("module-template.xml");
    if (!QFile::exists(newFilePath))
    {
        // Открываем шаблон
        auto tempFile = new QFile(templateFilePath, this);
        if (tempFile->open(QIODevice::ReadOnly))
        {
            // Читаем данные и форматируем
            QTextStream in(tempFile);
            auto formated = in.readAll();
            for (auto &argument : creationData)
                formated = formated.arg(argument);
            tempFile->close();
            in.reset();

            // Открываем новый файл
            auto newFile = new QFile(newFilePath, this);
            if (newFile->open(QIODevice::ReadWrite))
            {
                // Записываем отформатированные данные
                in.setDevice(newFile);
                in << formated;
                newFile->close();
            }
            newFile->deleteLater();
        }
        tempFile->deleteLater();
    }
}

/// \brief Переименовать файл (если было изменено имя устройства).
void DataController::renameFile(const QString &oldName, const QString &newName)
{
    auto oldFilepath = getFilePath(oldName);
    auto newFilepath = getFilePath(newName);
    if (QFile::exists(oldFilepath) && !QFile::exists(newFilepath))
    {
        if (!QFile::rename(oldFilepath, newFilepath))
            EMessageBox::error(nullptr, "Не получилось переименовать файл!");
    }
    else
        EMessageBox::error(nullptr, "Не получилось переименовать файл!");
}

/// \brief Удалить файл (модуль был удалён).
void DataController::removeFile(const QString &filename)
{
    // Если файл существует
    auto filepath = getFilePath(filename);
    if (QFile::exists(filepath))
    {
        // Перемещаем его в корзину (not hard delete!)
        if (!QFile::moveToTrash(filepath))
            EMessageBox::error(nullptr, "Не получилось удалить файл!");
    }
}

void DataController::saveFile(MasterModel *masterModel, XmlModel *slaveModel)
{
    // Создание документа
    auto doc = new QDomDocument();
    auto instructions = doc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    doc->appendChild(instructions);
    // Создаём из моделей ноды xml-документа
    auto moduleNode = masterModel->toNode(*doc, changedRow);
    doc->appendChild(*moduleNode);
    auto resNode = slaveModel->toNode(*doc);
    if (resNode != nullptr)
        moduleNode->appendChild(*resNode);

    // Запись в файл
    auto testFile = new QFile(getFilePath("test.xml"), this);
    if (testFile->open(QIODevice::ReadWrite))
    {
        QTextStream out(testFile);
        doc->save(out, 4);
        // out << doc->toString(4);
        testFile->close();
    }
}
