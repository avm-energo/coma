#include "xml/xmleditor/datacontroller.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <gen/stdfunc.h>
#include <widgets/epopup.h>
#include <xml/xmleditor/models/mastermodel.h>
#include <xml/xmleditor/models/xmlmodel.h>
#include <xml/xmltags.h>

DataController::DataController(QObject *parent) : QObject(parent), isModuleChanged(false), changedRow(-1)
{
}

QString DataController::getFilePath(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    return dir.filePath(filename);
}

void DataController::replaceChild(QDomDocument &doc, QDomElement &parent, const QString &tag, const QVariant &data)
{
    if (data.isValid() && data.canConvert<QString>())
    {
        auto oldNode = parent.firstChildElement(tag);
        auto newNode = doc.createElement(tag);
        auto newData = doc.createTextNode(data.value<QString>());
        newNode.appendChild(newData);
        if (!oldNode.isNull())
            parent.replaceChild(newNode, oldNode);
        else
            parent.appendChild(newNode);
    }
    else
        qWarning() << "Invalid QVariant data received!";
}

void DataController::replaceAttrs(
    QDomDocument &doc, QDomElement &node, const QStringList attrs, const QList<QVariant> &vals)
{
    if (attrs.size() == vals.size())
    {
        for (auto i = 0; i < vals.size(); i++)
        {
            if (vals[i].isValid() && vals[i].canConvert<QString>())
            {
                node.removeAttribute(attrs[i]);          // Удалить старый атрибут
                auto attrVal = vals[i].value<QString>(); // Получить новое значение атрибута
                auto newAttr = doc.createAttribute(attrs[i]); // Создать новый атрибут
                newAttr.setValue(attrVal);      // Установить значение новому атрибуту
                node.setAttributeNode(newAttr); // Установить узлу новый
            }
            else
                qWarning() << "Invalid QVariant data received!";
        }
    }
    else
        qWarning() << "Invalid data received!";
}

void DataController::writeToFile(const QDomDocument &doc, const QString &oldName, const QString &newName)
{
    // Запись в файл
    auto file = new QFile(getFilePath(oldName), this);
    file->resize(0);
    if (file->open(QIODevice::WriteOnly))
    {
        QTextStream out(file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out.setCodec(QTextCodec::codecForName("UTF-8"));
#else
        out.setEncoding(QStringConverter::Utf8);
#endif
        doc.save(out, 4);
        file->close();
    }
    file->deleteLater();
    // Если имя было изменено, то переименовываем файл
    if (oldName != newName)
        renameFile(oldName, newName);
}

void DataController::resetOrSaved()
{
    isModuleChanged = false;
}

bool DataController::getModuleState() const
{
    return isModuleChanged;
}

void DataController::configChanged()
{
    if (!isModuleChanged)
    {
        isModuleChanged = true;
        emit highlightModified();
    }
}

int DataController::getRow() const
{
    return changedRow;
}

void DataController::setRow(const int &row)
{
    changedRow = row;
}

void DataController::createFile(const QStringList &creationData)
{
    constexpr char templateName[] = "module-template.xml";
    auto newFilePath = getFilePath(creationData.last());
    auto templateFilePath = getFilePath(templateName);
    if (!QFile::exists(newFilePath))
    {
        // Открываем шаблон
        auto tempFile = new QFile(templateFilePath, this);
        if (tempFile->open(QIODevice::ReadOnly))
        {
            // Читаем данные и форматируем
            QTextStream in(tempFile);
            auto formatted = in.readAll();
            for (auto &argument : creationData)
                formatted = formatted.arg(argument);
            tempFile->close();
            in.reset();

            // Открываем новый файл
            auto newFile = new QFile(newFilePath, this);
            if (newFile->open(QIODevice::ReadWrite))
            {
                // Записываем отформатированные данные
                in.setDevice(newFile);
                in << formatted;
                newFile->close();
            }
            newFile->deleteLater();
        }
        else
            qCritical() << "Can't open file " << templateName;
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        // Перемещаем файл в корзину
        if (!QFile::moveToTrash(filepath))
#else
        // Hard delete file
        if (!QFile::remove(filepath))
#endif
            EMessageBox::error(nullptr, "Не получилось удалить файл!");
    }
}

void DataController::saveFile(MasterModel *masterModel, XmlModel *slaveModel)
{
    // Создание документа
    QDomDocument doc;
    auto instructions = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    doc.appendChild(instructions);
    // Создаём из моделей ноды xml-документа
    auto moduleNode = masterModel->toNode(doc, changedRow);
    auto resNode = slaveModel->toNode(doc);
    if (!moduleNode.isNull()) // module node not null for modules xml files
    {
        moduleNode.appendChild(resNode);
        doc.appendChild(moduleNode);
    }
    else // module node is null for s2file.xml
        doc.appendChild(resNode);
    // Запись в файл
    auto oldFileName = masterModel->data(masterModel->index(changedRow, 0), FilenameDataRole).value<QString>();
    auto newFileName = masterModel->data(masterModel->index(changedRow, 4)).value<QString>();
    writeToFile(doc, oldFileName, newFileName);
}

void DataController::saveFile(MasterModel *masterModel)
{
    QDomDocument doc;
    auto oldFileName = masterModel->data(masterModel->index(changedRow, 0), FilenameDataRole).value<QString>();
    auto file = new QFile(getFilePath(oldFileName), this);
    if (file->open(QIODevice::ReadOnly))
    {
        if (doc.setContent(file))
        {
            auto moduleNode = doc.firstChildElement(tags::module);
            if (!moduleNode.isNull())
            {
                auto data = {
                    masterModel->data(masterModel->index(changedRow, 1)), //
                    masterModel->data(masterModel->index(changedRow, 2))  //
                };
                replaceAttrs(doc, moduleNode, { tags::mtypeb, tags::mtypem }, data);
                replaceChild(doc, moduleNode, tags::name, masterModel->data(masterModel->index(changedRow, 0)));
                replaceChild(doc, moduleNode, tags::version, masterModel->data(masterModel->index(changedRow, 3)));
            }
        }
        file->close();
    }
    file->deleteLater();
    auto newFileName = masterModel->data(masterModel->index(changedRow, 4)).value<QString>();
    writeToFile(doc, oldFileName, newFileName);
}
