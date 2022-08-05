#include "datacontroller.h"

#include "../../gen/stdfunc.h"
#include "../../widgets/epopup.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

DataController::DataController(QObject *parent) : QObject(parent), isModuleChanged(false), changedRow(-1)
{
}

QString DataController::getFilePath(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    return dir.filePath(filename);
}

void DataController::resetOrSaved()
{
    isModuleChanged = false;
}

bool DataController::getModuleState() const
{
    return isModuleChanged;
}

/// \brief Сохраняет номер строки изменённого конфига в мастер модели
int DataController::getRow() const
{
    return changedRow;
}

/// \brief Возвращает сохранённую строку изменённого конфига
void DataController::setRow(const int &row)
{
    changedRow = row;
}

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

void DataController::removeFile(const QString &filename)
{
    // Если файл существует
    auto filepath = getFilePath(filename);
    if (QFile::exists(filepath))
    {
        // Перемещаем его в корзину (не хард делитим!)
        if (!QFile::moveToTrash(filepath))
            EMessageBox::error(nullptr, "Не получилось удалить файл!");
    }
}

void DataController::configChanged()
{
    if (!isModuleChanged)
    {
        isModuleChanged = true;
        emit highlightModified();
    }
}
