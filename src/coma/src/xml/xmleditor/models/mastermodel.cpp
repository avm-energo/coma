#include "xml/xmleditor/models/mastermodel.h"

#include <gen/settings.h>
#include <xml/xmltags.h>

MasterModel::MasterModel(QObject *parent) : BaseEditorModel(1, 1, ModelType::Master, parent)
{
    readModulesToModel();
}

QDomElement MasterModel::toNode(QDomDocument &doc, const int &row)
{
    // Создаём основной узел
    auto name = data(index(row, 0), FilenameDataRole).toString();
    if (name.contains("s2files"))
        return QDomElement {};
    else
    {
        auto moduleNode = makeElement(doc, tags::module);
        const auto typeB = data(index(row, 1));
        setAttribute(doc, moduleNode, tags::mtypeb, typeB);
        setAttribute(doc, moduleNode, tags::mtypem, data(index(row, 2)));
        makeElement(doc, moduleNode, tags::name, data(index(row, 0)));
        // Для мезонинных плат не сохраняем ноду <version>
        if (typeB.value<QString>() != "00")
            makeElement(doc, moduleNode, tags::version, data(index(row, 3)));
        auto featuresElem = makeGroup(doc, moduleNode, tags::features); // create group "Features"
        makeElement(doc, featuresElem, tags::isboxmodule, data(index(row, 5)));
        makeElement(doc, featuresElem, tags::isrtcexist, data(index(row, 6)));
        return moduleNode;
    }
}

void MasterModel::undoChanges(const int &row, const bool &openState)
{
    auto fileName = data(index(row, 0), FilenameDataRole).value<QString>();
    auto dir = QDir(Settings::configDir());
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

void MasterModel::readModulesToModel()
{
    // Создание и настройка модели для Master View
    auto dir = QDir(Settings::configDir());
    auto modules = dir.entryList({ "*.xml" }, QDir::Files);
    setRowCount(0);
    setColumnCount(7);
    // Каждый xml-файл считывается в модель
    for (const auto &name : modules)
    {
        // Фильтр для шаблонного файла
        if (name.contains("template"))
            continue;
        // Resize model
        auto row = rowCount();
        setRowCount(row + 1);

        auto moduleFile = new QFile(dir.filePath(name));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            QDomDocument domDoc;
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
            QString errMsg = "";
            auto line = 0, column = 0;
            if (domDoc.setContent(moduleFile, &errMsg, &line, &column))
#else
            QDomDocument::ParseResult result = domDoc.setContent(moduleFile);
            if (result.errorMessage.isEmpty())
#endif
            {
                auto domElement = domDoc.documentElement();
                parseXmlNode(domElement, name, row);
                setData(index(row, 0), name, FilenameDataRole);
            }
            // Если QtXml парсер не смог корректно считать xml файл
            else
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
                qWarning() << errMsg << " File: " << name << " Line: " << line << " Column: " << column;
#else
                qWarning() << result.errorMessage << " File: " << name << " Line: " << result.errorLine
                           << " Column: " << result.errorColumn;
#endif
            moduleFile->close();
        }
        moduleFile->deleteLater();
    }
}

void MasterModel::parseXmlNode(const QDomNode &node, const QString &filename, const int &row)
{
    // Устанавливаем имя файла
    setData(index(row, 4), filename);
    if (filename.contains("s2files"))
    {
        setData(index(row, 0), "S2 Files");
        setData(index(row, 1), "-");
        setData(index(row, 2), "-");
        setData(index(row, 3), "No version");
    }
    else
    {
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
            auto featuresNode = domElModule.firstChildElement(tags::features);
            if (!featuresNode.isNull())
            {
                auto child = featuresNode.firstChild();
                while (!child.isNull())
                {
                    if (!child.isComment() && child.isElement())
                    {
                        auto featuresNodeElement = child.toElement();
                        if (!featuresNodeElement.isNull())
                        {
                            const QString key = featuresNodeElement.tagName();
                            const QString value = featuresNodeElement.text();
                            if (key == tags::isboxmodule)
                                setData(index(row, 5), value);
                            if (key == tags::isrtcexist)
                                setData(index(row, 6), value);
                        }
                    }
                    child = child.nextSibling();
                }
            }
        }
    }
}

QStringList MasterModel::getNewList(const QStringList &saved)
{
    auto filename = saved[1].toLower() + saved[2].toLower() + ".xml";
    auto newSaved = saved;
    newSaved.append(filename);
    return newSaved;
}

void MasterModel::masterItemSelected(const QModelIndex &itemIndex)
{
    const auto row = itemIndex.row();
    auto dataFilename = data(index(row, 0), FilenameDataRole);
    if (dataFilename.canConvert<QString>())
    {
        QDir homeDir(Settings::configDir());
        auto filename = qvariant_cast<QString>(dataFilename);
        auto moduleFile = new QFile(homeDir.filePath(filename), this);
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            QDomDocument domDoc;
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
            QString errMsg = "";
            auto line = 0, column = 0;
            if (domDoc.setContent(moduleFile, &errMsg, &line, &column))
#else
            QDomDocument::ParseResult result = domDoc.setContent(moduleFile);
            if (result.errorMessage.isEmpty())
#endif
            {
                auto domElement = domDoc.documentElement();
                emit itemSelected(domElement);
            }
            // Если QtXml парсер не смог корректно считать xml файл
            else
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
                qWarning() << errMsg << " File: " << filename << " Line: " << line << " Column: " << column;
#else
                qWarning() << result.errorMessage << " File: " << filename << " Line: " << result.errorLine
                           << " Column: " << result.errorColumn;
#endif
            moduleFile->close();
        }
        moduleFile->deleteLater();
    }
}

void MasterModel::create(const QStringList &saved, int *row)
{
    auto newSaved = getNewList(saved);
    BaseEditorModel::create(newSaved, row);
    setData(index(*row, 0), newSaved.last(), FilenameDataRole);
    emit createFile(newSaved);
}

void MasterModel::update(const QStringList &saved, const int row)
{
    auto filename = data(index(row, 0), FilenameDataRole).toString();
    if (!filename.contains("s2files"))
    {
        auto newSaved = getNewList(saved);
        BaseEditorModel::update(newSaved, row);
        emit modelChanged();
    }
    else
        emit error("s2files.xml нельзя изменить в этом окне!");
}

void MasterModel::remove(const int row)
{
    auto filename = data(index(row, 0), FilenameDataRole).toString();
    if (!filename.contains("s2files"))
    {
        BaseEditorModel::remove(row);
        emit removeFile(filename);
    }
    else
        emit error("s2files.xml нельзя удалить");
}
