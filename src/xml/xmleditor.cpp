#include "xmleditor.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "xmlconfigparser.h"

#include <QPushButton>
#include <QStringList>
#include <QToolBar>
#include <utility>

std::pair<QDomNode, QStandardItem *> ParseXmlAddCheckSubgroups(
    QDomNode &checkNode, int &index, QStandardItem *parent, QString &name);
void ParseXmlCheckChildrens(QDomNode &checkNode, int &index, QStandardItem *parent, QString name);

XmlEditor::XmlEditor(QWidget *parent) : QDialog(parent, Qt::Window), slaveModel(nullptr), masterModel(nullptr)
{
    if (parent != nullptr)
    {
        SetupUI(parent->size());
        ReadModulesToMasterModel();
        XmlConfigParser::ParseS2ConfigToMap();
        this->exec();
    }
}

void XmlEditor::SetupUI(QSize pSize)
{
    this->setGeometry(0, 0, pSize.width(), pSize.height());
    this->setWindowTitle("Modules Editor");
    auto mainLayout = new QHBoxLayout(this);

    // Добавление рабочих пространств на основной слой
    mainLayout->addLayout(GetWorkspace(WorkspaceType::Master));
    mainLayout->addLayout(GetWorkspace(WorkspaceType::Slave));
    this->setLayout(mainLayout);
}

void XmlEditor::Close()
{
    this->hide();
}

QVBoxLayout *XmlEditor::GetWorkspace(WorkspaceType type)
{
    // Создание рабочего пространства
    auto workspace = new QVBoxLayout();
    workspace->setContentsMargins(5, 5, 5, 5);

    // Настройка тулбара
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(30, 30));
    if (type == Master)
    {
        toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать модуль", this, &XmlEditor::Close);
        toolbar->addSeparator();
        toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить модуль", this, &XmlEditor::Close);
    }
    else
    {
        toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать свойство", this, &XmlEditor::Close);
        toolbar->addSeparator();
        toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить свойство", this, &XmlEditor::Close);
    }
    workspace->addWidget(toolbar);

    if (type == Master)
    {
        // Создание и настройка QTableView
        masterView = new QTableView(this);
        masterView->setSortingEnabled(true);
        QObject::connect(masterView, &QTableView::clicked, this, &XmlEditor::MasterItemSelected);
        masterView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        masterView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        workspace->addWidget(masterView);
    }
    else
    {
        // Создание и настройка QTreeView
        slaveView = new QTreeView(this);
        slaveView->setSortingEnabled(true);
        workspace->addWidget(slaveView);
    }

    return workspace;
}

void XmlEditor::ReadModulesToMasterModel()
{
    // Создание и настройка модели для master
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    auto modules = dir.entryList(QDir::Files).filter(".xml");
    if (masterModel == nullptr)
        masterModel = CreateMasterModel(modules.count());
    masterView->setModel(masterModel);

    // Каждый xml-файл считывается в модель
    for (int i = 0; i < modules.count(); i++)
    {
        auto domDoc = new QDomDocument;
        auto moduleFile = new QFile(dir.filePath(modules[i]));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            if (domDoc->setContent(moduleFile))
            {
                auto domElement = domDoc->documentElement();
                ParseXmlToMasterModel(domElement, modules[i], i);
            }
            moduleFile->close();
        }
        delete domDoc;
        delete moduleFile;
    }
}

QStandardItemModel *XmlEditor::CreateMasterModel(const int rows)
{
    const int cols = 5;
    auto model = new QStandardItemModel(rows, cols);
    model->setHeaderData(0, Qt::Horizontal, "Файл");
    model->setHeaderData(1, Qt::Horizontal, "Устройство");
    model->setHeaderData(2, Qt::Horizontal, "Type B");
    model->setHeaderData(3, Qt::Horizontal, "Type M");
    model->setHeaderData(4, Qt::Horizontal, "Версия");
    return model;
}

QStandardItemModel *XmlEditor::CreateSlaveModel()
{
    auto model = new QStandardItemModel;
    model->setColumnCount(1);
    model->setHeaderData(0, Qt::Horizontal, "Свойства");
    return model;
}

void XmlEditor::ParseXmlToMasterModel(const QDomNode &node, const QString &filename, int &index)
{
    // Устанавливаем имя файла
    auto indexFilename = masterModel->index(index, 0);
    masterModel->setData(indexFilename, filename);

    auto domElModule = node.firstChildElement("module");
    if (!domElModule.isNull())
    {
        // Получаем аттрибуты TypeB и TypeM
        auto indexTypeB = masterModel->index(index, 2);
        auto indexTypeM = masterModel->index(index, 3);
        masterModel->setData(indexTypeM, domElModule.attribute("mtypem", "00"));
        masterModel->setData(indexTypeB, domElModule.attribute("mtypeb", "00"));

        // Получаем имя модуля
        auto domElName = domElModule.firstChildElement("name");
        if (!domElName.isNull())
        {
            auto indexName = masterModel->index(index, 1);
            masterModel->setData(indexName, domElName.text());
        }

        // И его версию
        auto domElVersion = domElModule.firstChildElement("version");
        if (!domElVersion.isNull())
        {
            auto indexVersion = masterModel->index(index, 4);
            masterModel->setData(indexVersion, domElVersion.text());
        }
    }
}

void XmlEditor::MasterItemSelected(const QModelIndex &index)
{
    const auto row = index.row();
    auto indexFilename = masterModel->index(row, 0);
    auto dataFilename = masterModel->data(indexFilename);
    if (dataFilename.canConvert<QString>())
    {
        auto module = (qvariant_cast<QString>(dataFilename));
        QDir homeDir(StdFunc::GetSystemHomeDir());

        if (slaveModel == nullptr)
        {
            slaveModel = CreateSlaveModel();
            slaveView->setModel(slaveModel);
        }
        else
            slaveModel->clear();

        auto domDoc = new QDomDocument;
        auto moduleFile = new QFile(homeDir.filePath(module));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            if (domDoc->setContent(moduleFile))
            {
                auto domElement = domDoc->documentElement().firstChild();
                ParseXmlToSlaveModel(domElement, 0, nullptr);
            }
            moduleFile->close();
        }
        delete domDoc;
        delete moduleFile;
    }
}

void XmlEditor::ParseXmlToSlaveModel(QDomNode &node, int index, QStandardItem *parent)
{
    while (!node.isNull())
    {
        QStandardItem *element = nullptr;
        auto newIndex = 0;
        auto state = true;
        if (node.isElement() && !node.isComment())
        {
            auto name = node.nodeName();

            // Парсим ноду check
            if (name == "check")
            {
                state = ParseXmlCheck(node, index, parent);
            }

            if (state)
            {
                element = new QStandardItem(name);
                if (parent == nullptr)
                {
                    parent = element;
                    slaveModel->setItem(0, parent);
                }
                else
                {
                    parent->setChild(index, element);
                    index++;
                }
                newIndex = ParseXmlFindAllAttributes(node, element);
            }
        }
        else if (node.isText())
        {
            auto text = node.toText();
            if (!text.isNull())
            {
                if (parent != nullptr)
                {
                    element = new QStandardItem(text.data());
                    parent->setChild(0, element);
                }
            }
            break;
        }

        // Делаем это всё рекурсивно
        if (state)
        {
            auto cnode = node.firstChild();
            ParseXmlToSlaveModel(cnode, newIndex, element);
        }
        node = node.nextSibling();
    }
}

int XmlEditor::ParseXmlFindAllAttributes(QDomNode &domNode, QStandardItem *element)
{
    // Поиск аттрибутов
    auto domElement = domNode.toElement();
    if (!domElement.isNull())
    {
        auto attrs = domElement.attributes();
        if (attrs.count() > 0)
        {
            auto attributes = new QStandardItem("attributes");
            element->setChild(0, attributes);
            for (int i = 0; i < attrs.count(); i++)
            {
                auto attr = attrs.item(i).toAttr();
                auto aName = new QStandardItem(attr.name());
                attributes->setChild(i, aName);
                auto aValue = new QStandardItem(attr.value());
                aName->setChild(0, aValue);
            }
            return 1;
        }
    }
    return 0;
}

bool XmlEditor::ParseXmlCheck(QDomNode &checkNode, int &index, QStandardItem *parent)
{
    // Парсим имя
    QString name = "";
    auto header = checkNode.attributes().namedItem("header");
    if (!header.isNull())
        name = header.toAttr().value();
    else
        name = checkNode.nodeName();

    if (parent != nullptr)
    {
        auto element = new QStandardItem(name);
        parent->setChild(index, element);
        index++;

        int newIndex = 0;
        ParseXmlCheckChildrens(checkNode, newIndex, element, "groups");
        ParseXmlCheckChildrens(checkNode, newIndex, element, "signals");
        ParseXmlCheckChildrens(checkNode, newIndex, element, "records");
    }

    return false;
}

std::pair<QDomNode, QStandardItem *> ParseXmlAddCheckSubgroups(
    QDomNode &checkNode, int &index, QStandardItem *parent, QString &name)
{
    auto checkSubgroup = checkNode.firstChildElement(name);
    if (!checkSubgroup.isNull() && parent != nullptr)
    {
        auto group = new QStandardItem(checkSubgroup.nodeName());
        parent->setChild(index, group);
        index++;
        return std::pair(checkSubgroup, group);
    }
    else
        return std::pair(QDomNode(), nullptr);
}

void ParseXmlCheckChildrens(QDomNode &checkNode, int &index, QStandardItem *parent, QString name)
{
    auto result = ParseXmlAddCheckSubgroups(checkNode, index, parent, name);
    auto groupNode = result.first;
    auto newParent = result.second;
    if (groupNode.hasChildNodes() && newParent != nullptr)
    {
        int newIndex = 0;
        auto groupElement = groupNode.firstChild();
        while (!groupElement.isNull())
        {
            if (name == "groups")
            {
                // парсим элемент из groups
            }
            else if (name == "signals")
            {
                // парсим элемент из signals
            }
            else if (name == "records")
            {
                // парсим элемент из records
            }
            else
            {
                // unexpected :D
            }
            groupElement = groupElement.nextSibling();
        }
    }
    return;
}
