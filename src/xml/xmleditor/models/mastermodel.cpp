#include "mastermodel.h"

#include <gen/stdfunc.h>

MasterModel::MasterModel(QObject *parent) : BaseEditorModel(1, 1, ModelType::Master, parent)
{
    readModulesToModel();
}

/// \brief Creates XML DOM node at base of content selected item.
QDomElement MasterModel::toNode(QDomDocument &doc, const int &row)
{
    // Создаём основной узел
    auto moduleNode = makeElement(doc, tags::module);
    const auto typeB = data(index(row, 1));
    setAttribute(doc, moduleNode, tags::mtypeb, typeB);
    setAttribute(doc, moduleNode, tags::mtypem, data(index(row, 2)));
    makeElement(doc, moduleNode, tags::name, data(index(row, 0)));
    // Для мезонинных плат не сохраняем ноду <version>
    if (typeB.value<QString>() != "00")
        makeElement(doc, moduleNode, tags::version, data(index(row, 3)));
    return moduleNode;
}

/// \brief Undo changes of opened XML model.
void MasterModel::undoChanges(const int &row, const bool &openState)
{
    auto fileName = data(index(row, 0), FilenameDataRole).value<QString>();
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    // Заново парсим файл в модель
    auto file = new QFile(dir.filePath(fileName));
    if (file->open(QIODevice::ReadOnly))
    {
        QDomDocument doc;
        if (doc.setContent(file))
        {
            auto root = doc.documentElement();
            parseXmlNode(root, fileName, row);
        }
        file->close();
    }
    file->deleteLater();
    // Если файл модуля был открыт в правой view
    if (openState)
        masterItemSelected(index(row, 0));
}

/// \brief Initial function for reading existing configuration files in model.
void MasterModel::readModulesToModel()
{
    // Создание и настройка модели для Master View
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    auto modules = dir.entryList(QDir::Files).filter(".xml");
    setRowCount(modules.count());
    setColumnCount(5);
    // Каждый xml-файл считывается в модель
    for (int i = 0; i < modules.count(); i++)
    {
        // Фильтр для s2files и шаблонного файла
        const auto &name = modules[i];
        if (name.contains("template") || name.contains("s2files"))
        {
            setRowCount(rowCount() - 1);
            continue;
        }

        auto moduleFile = new QFile(dir.filePath(name));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            QDomDocument domDoc;
            QString errMsg = "";
            auto line = 0, column = 0;
            if (domDoc.setContent(moduleFile, &errMsg, &line, &column))
            {
                auto domElement = domDoc.documentElement();
                parseXmlNode(domElement, modules[i], i);
                setData(index(i, 0), modules[i], FilenameDataRole);
            }
            // Если QtXml парсер не смог корректно считать xml файл
            else
                qWarning() << errMsg << " File: " << name << " Line: " << line << " Column: " << column;
            moduleFile->close();
        }
        moduleFile->deleteLater();
    }
}

/// \brief Parsing each file's content in model.
void MasterModel::parseXmlNode(const QDomNode &node, const QString &filename, const int &row)
{
    // Устанавливаем имя файла
    setData(index(row, 4), filename);
    auto domElModule = node.toElement();
    if (!domElModule.isNull())
    {
        // Получаем аттрибуты TypeB и TypeM
        setData(index(row, 1), domElModule.attribute(tags::mtypeb, "00"));
        setData(index(row, 2), domElModule.attribute(tags::mtypem, "00"));
        // Получаем имя модуля
        auto domElName = domElModule.firstChildElement(tags::name);
        if (!domElName.isNull())
            setData(index(row, 0), domElName.text());
        // И его версию
        auto domElVersion = domElModule.firstChildElement(tags::version);
        if (domElVersion.isNull())
            setData(index(row, 3), "No version");
        else
            setData(index(row, 3), domElVersion.text());
    }
}

/// \brief Updates a data from a dialog.
QStringList MasterModel::getNewList(const QStringList &saved)
{
    auto filename = saved[1].toLower() + saved[2].toLower() + ".xml";
    auto newSaved = saved;
    newSaved.append(filename);
    return newSaved;
}

/// \brief Creates XML model in slave workspace where user select item in master model.
void MasterModel::masterItemSelected(const QModelIndex &itemIndex)
{
    const auto row = itemIndex.row();
    auto dataFilename = data(index(row, 0), FilenameDataRole);
    if (dataFilename.canConvert<QString>())
    {
        QDir homeDir(StdFunc::GetSystemHomeDir());
        auto filename = qvariant_cast<QString>(dataFilename);
        auto moduleFile = new QFile(homeDir.filePath(filename), this);
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            QDomDocument domDoc;
            QString errMsg = "";
            auto line = 0, column = 0;
            if (domDoc.setContent(moduleFile, &errMsg, &line, &column))
            {
                auto domElement = domDoc.documentElement();
                emit itemSelected(domElement);
            }
            // Если QtXml парсер не смог корректно считать xml файл
            else
                qWarning() << errMsg << " File: " << filename << " Line: " << line << " Column: " << column;
            moduleFile->close();
        }
        moduleFile->deleteLater();
    }
}

/// \brief Slot for inserting a new item in the model.
void MasterModel::create(const QStringList &saved, int *row)
{
    auto newSaved = getNewList(saved);
    BaseEditorModel::create(newSaved, row);
    setData(index(*row, 0), newSaved.last(), FilenameDataRole);
    emit createFile(newSaved);
}

/// \brief Slot for updating an item's data in the model.
void MasterModel::update(const QStringList &saved, const int row)
{
    auto newSaved = getNewList(saved);
    BaseEditorModel::update(newSaved, row);
    emit modelChanged();
}

/// \brief Slot for deleting an exisiting item in the model.
void MasterModel::remove(const int row)
{
    auto filename = data(index(row, 0), FilenameDataRole).value<QString>();
    BaseEditorModel::remove(row);
    emit removeFile(filename);
}
