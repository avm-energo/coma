#include "mastermodel.h"

#include "../../../gen/stdfunc.h"

MasterModel::MasterModel(QObject *parent) : IEditorModel(1, 1, ModelType::Master, parent)
{
    readModulesToModel();
}

QDomElement *MasterModel::toNode(QDomDocument &doc, const int row)
{
    auto moduleNode = makeElement(doc, "module");
    auto btypeVar = data(index(row, 1));
    auto mtypeVar = data(index(row, 2));
    auto nameVar = data(index(row, 0));
    auto versionVar = data(index(row, 3));

    setAttribute(doc, *moduleNode, "mtypeb", btypeVar);
    setAttribute(doc, *moduleNode, "mtypem", mtypeVar);
    moduleNode->appendChild(*makeElement(doc, "name", nameVar));
    moduleNode->appendChild(*makeElement(doc, "version", versionVar));

    return moduleNode;
}

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
        auto domDoc = new QDomDocument;
        auto moduleFile = new QFile(dir.filePath(modules[i]));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            QString errMsg = "";
            int line = 0, column = 0;
            if (domDoc->setContent(moduleFile, &errMsg, &line, &column))
            {
                auto domElement = domDoc->documentElement();
                parseXmlNode(domElement, modules[i], i);
            }
            // Если QtXml парсер не смог корректно считать xml файл
            else
                qWarning() << errMsg << " Line: " << line << " Column: " << column;
            moduleFile->close();
        }
        delete domDoc;
        delete moduleFile;
    }
}

void MasterModel::parseXmlNode(const QDomNode &node, const QString &filename, int &row)
{
    // Устанавливаем имя файла
    auto indexFilename = this->index(row, 4);
    setData(indexFilename, filename);
    auto domElModule = node.toElement();
    if (!domElModule.isNull())
    {
        // Получаем аттрибуты TypeB и TypeM
        auto indexTypeB = this->index(row, 1);
        auto indexTypeM = this->index(row, 2);
        setData(indexTypeM, domElModule.attribute("mtypem", "00"));
        setData(indexTypeB, domElModule.attribute("mtypeb", "00"));

        // Получаем имя модуля
        auto domElName = domElModule.firstChildElement("name");
        if (!domElName.isNull())
        {
            auto indexName = this->index(row, 0);
            setData(indexName, domElName.text());
        }

        // И его версию
        auto domElVersion = domElModule.firstChildElement("version");
        if (!domElVersion.isNull())
        {
            auto indexVersion = this->index(row, 3);
            setData(indexVersion, domElVersion.text());
        }
    }
}

QStringList MasterModel::getNewList(const QStringList &saved)
{
    auto filename = "avtuk-" + saved[1].toLower() + saved[2].toLower() + ".xml";
    auto newSaved = saved;
    newSaved.append(filename);
    return newSaved;
}

void MasterModel::masterItemSelected(const QModelIndex &index)
{
    const auto row = index.row();
    auto indexFilename = this->index(row, 4);
    auto dataFilename = data(indexFilename);
    if (dataFilename.canConvert<QString>())
    {
        auto domDoc = new QDomDocument;
        QDir homeDir(StdFunc::GetSystemHomeDir());
        auto filename = qvariant_cast<QString>(dataFilename);
        auto moduleFile = new QFile(homeDir.filePath(filename), this);
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            QString errMsg = "";
            int line = 0, column = 0;
            if (domDoc->setContent(moduleFile, &errMsg, &line, &column))
            {
                auto domElement = domDoc->documentElement();
                emit itemSelected(domElement);
            }
            // Если QtXml парсер не смог корректно считать xml файл
            else
                qWarning() << errMsg << " Line: " << line << " Column: " << column;
            moduleFile->close();
        }
        delete domDoc;
        moduleFile->deleteLater();
    }
}

void MasterModel::create(const QStringList &saved, int *row)
{
    auto newSaved = getNewList(saved);
    IEditorModel::create(newSaved, row);
    emit createFile(newSaved);
}

void MasterModel::update(const QStringList &saved, const int &row)
{
    auto newSaved = getNewList(saved);
    IEditorModel::update(newSaved, row);
    emit modelChanged();

    // TODO: This after must doing after accepting saving changes into file
    //    auto oldName = data(index(row, 4)).value<QString>();
    //    auto newName = newSaved.last();
    //    if (oldName != newName)
    //        emit renameFile(oldName, newName);
}

void MasterModel::remove(const int &row)
{
    auto filename = data(index(row, 4)).value<QString>();
    emit removeFile(filename);
    IEditorModel::remove(row);
}
