#include "mastermodel.h"

#include "../../../gen/stdfunc.h"

MasterModel::MasterModel(QObject *parent) : QStandardItemModel(1, 1, parent)
{
    readModulesToModel();
}

void MasterModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    int column = 0;
    for (auto &label : labels)
    {
        setHeaderData(column, Qt::Horizontal, label);
        column++;
    }
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
        auto moduleFile = new QFile(homeDir.filePath(filename));
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
        delete moduleFile;
    }
}

/// \brief Слот который принимает запрос от диалога и отправляет сигнал с ответом
void MasterModel::getDialogRequest(const int &row)
{
    if (row >= 0 && row < rowCount())
    {
        QStringList retList;
        auto cols = columnCount();
        retList.reserve(cols);
        // Собираем данные
        for (auto column = 0; column < cols; column++)
        {
            auto itemIndex = index(row, column);
            auto item = data(itemIndex);
            if (item.isValid() && item.canConvert<QString>())
            {
                auto itemStr = item.value<QString>();
                retList.append(itemStr);
            }
            else
                retList.append("");
        }
        // Отправляем сигнал с ответом
        emit sendDialogResponse(retList);
    }
}

void MasterModel::create(const QStringList &saved, int *row)
{
    *row = rowCount();
    QList<QStandardItem *> items;
    for (const auto &item : saved)
        items.append(new QStandardItem(item));
    insertRow(rowCount(), items);
    // TODO: Create the file - controller work
}

void MasterModel::update(const QStringList &saved, const int &row)
{
    if (row >= 0 && row < rowCount())
    {
        auto cols = saved.count();
        // Обновляем данные
        for (auto column = 0; column < cols; column++)
        {
            auto itemIndex = index(row, column);
            setData(itemIndex, saved[column]);
        }
    }
    // TODO: Rename file if B or M changed - controller work
}

void MasterModel::remove(const int &row)
{
    removeRow(row);
    // TODO: Remove the file after - controller work
}
