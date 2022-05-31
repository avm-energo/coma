#include "moduleseditor.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "xmlconfigparser.h"

#include <QPushButton>
#include <QStringList>
#include <QToolBar>

constexpr char resourceDirectory[] = ":/module";

ModulesEditor::ModulesEditor(QWidget *parent) : QDialog(parent, Qt::Window), slaveModel(nullptr), masterModel(nullptr)
{
    if (parent != nullptr)
    {
        SetupUI(parent->size());
        ReadModulesToMasterModel();
        XmlConfigParser::ParseS2ConfigToMap();
        this->exec();
    }
}

void ModulesEditor::SetupUI(QSize pSize)
{
    this->setGeometry(0, 0, pSize.width(), pSize.height());
    this->setWindowTitle("Modules Editor");
    auto mainLayout = new QHBoxLayout(this);

    // Добавление рабочих пространств на основной слой
    mainLayout->addLayout(GetWorkspace(WorkspaceType::Master));
    mainLayout->addLayout(GetWorkspace(WorkspaceType::Slave));
    this->setLayout(mainLayout);
}

void ModulesEditor::Close()
{
    this->hide();
}

QVBoxLayout *ModulesEditor::GetWorkspace(WorkspaceType type)
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
        toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать модуль", this, &ModulesEditor::Close);
        toolbar->addSeparator();
        toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить модуль", this, &ModulesEditor::Close);
    }
    else
    {
        toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать свойство", this, &ModulesEditor::Close);
        toolbar->addSeparator();
        toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить свойство", this, &ModulesEditor::Close);
    }
    workspace->addWidget(toolbar);

    if (type == Master)
    {
        // Создание и настройка QTableView
        masterView = new QTableView(this);
        masterView->setSortingEnabled(true);
        QObject::connect(masterView, &QTableView::clicked, this, &ModulesEditor::MasterItemSelected);
        masterView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        masterView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        workspace->addWidget(masterView);
    }
    else
    {
        // Создание и настройка QTreeView
        slaveView = new QTreeView(this);
        // qtw->setHeaderLabel("Свойства");
        slaveView->setSortingEnabled(true);
        workspace->addWidget(slaveView);
    }

    return workspace;
}

QDir ModulesEditor::UnpackData()
{
    QDir resDir(resourceDirectory);
    QDir homeDir(StdFunc::GetSystemHomeDir());
    auto xmlFiles = resDir.entryList(QDir::Files).filter(".xml");
    auto homeFiles = homeDir.entryList(QDir::Files).filter(".xml");

    // Копируем файлы из ресурсов в AppData/Local/AVM-Debug
    if (homeFiles.count() < xmlFiles.count())
    {
        foreach (QString filename, xmlFiles)
        {
            if (!QFile::copy(resDir.filePath(filename), homeDir.filePath(filename)))
            {
                qCritical() << Error::DescError;
                qInfo() << resDir.filePath(filename);
            }
        }
    }
    return homeDir;
}

void ModulesEditor::ReadModulesToMasterModel()
{
    // Создание и настройка модели для master
    auto dir = UnpackData();
    auto modules = dir.entryList(QDir::Files).filter(".xml");
    if (masterModel == nullptr)
        masterModel = CreateMasterModel(modules.count(), 4);
    masterView->setModel(masterModel);

    // Каждый xml-файл считывает в модель
    for (int i = 0; i < modules.count(); i++)
    {
        auto domDoc = new QDomDocument;
        auto moduleFile = new QFile(dir.filePath(modules[i]));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            if (domDoc->setContent(moduleFile))
            {
                auto domElement = domDoc->documentElement();
                ParseXmlToMasterModel(domElement, i);
            }
            moduleFile->close();
        }
        delete domDoc;
        delete moduleFile;
    }
}

QStandardItemModel *ModulesEditor::CreateMasterModel(const int rows, const int cols)
{
    auto model = new QStandardItemModel(rows, cols);
    model->setHeaderData(0, Qt::Horizontal, "Название");
    model->setHeaderData(1, Qt::Horizontal, "Type B");
    model->setHeaderData(2, Qt::Horizontal, "Type M");
    model->setHeaderData(3, Qt::Horizontal, "Версия");
    return model;
}

QStandardItemModel *ModulesEditor::CreateSlaveModel()
{
    auto model = new QStandardItemModel;
    model->setColumnCount(1);
    model->setHeaderData(0, Qt::Horizontal, "Свойства");
    return model;
}

void ModulesEditor::ParseXmlToMasterModel(const QDomNode &node, const int &index)
{
    auto domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            auto domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                // Получаем аттрибуты TypeB и TypeM
                if (domElement.tagName() == "module")
                {
                    auto indexTypeB = masterModel->index(index, 1);
                    auto indexTypeM = masterModel->index(index, 2);
                    masterModel->setData(indexTypeM, domElement.attribute("mtypem", "00"));
                    masterModel->setData(indexTypeB, domElement.attribute("mtypeb", "00"));
                }
                // Получаем имя модуля
                else if (domElement.tagName() == "name")
                {
                    auto indexName = masterModel->index(index, 0);
                    masterModel->setData(indexName, domElement.text());
                }
                // И его версию
                else if (domElement.tagName() == "version")
                {
                    auto indexVersion = masterModel->index(index, 3);
                    masterModel->setData(indexVersion, domElement.text());
                    break;
                }
            }
        }
        // Делаем это всё рекурсивно
        ParseXmlToMasterModel(domNode, index);
        domNode = domNode.nextSibling();
    }
}

void ModulesEditor::MasterItemSelected(const QModelIndex &index)
{
    const auto row = index.row();
    auto indexTypeB = masterModel->index(row, 1), indexTypeM = masterModel->index(row, 2);
    auto dataTypeB = masterModel->data(indexTypeB), dataTypeM = masterModel->data(indexTypeM);
    if (dataTypeB.canConvert<QString>() && dataTypeM.canConvert<QString>())
    {
        auto moduleType = (qvariant_cast<QString>(dataTypeB) + qvariant_cast<QString>(dataTypeM)).toLower();
        QDir homeDir(StdFunc::GetSystemHomeDir());
        auto module = homeDir.entryList(QDir::Files).filter(moduleType)[0];

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
                auto domElement = domDoc->documentElement();
                int index = 0;
                ParseXmlToSlaveModel(domElement, index, nullptr);
            }
            moduleFile->close();
        }
        delete domDoc;
        delete moduleFile;
    }
}

void ModulesEditor::ParseXmlToSlaveModel(QDomNode &node, int index, QStandardItem *parent)
{
    while (!node.isNull())
    {
        QStandardItem *element = nullptr;
        auto aState = false;
        if (node.isElement() && !node.isComment())
        {
            auto domElement = node.toElement();
            if (!domElement.isNull())
            {
                auto name = domElement.tagName();
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

                // Поиск аттрибутов
                auto attrs = domElement.attributes();
                if (attrs.count() > 0)
                {
                    aState = true;
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
                }
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
        auto newIndex = 0;
        if (aState)
            newIndex = 1;
        auto cnode = node.firstChild();
        ParseXmlToSlaveModel(cnode, newIndex, element);
        node = node.nextSibling();
    }
}
