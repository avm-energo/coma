#include "moduleseditor.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"

#include <QPushButton>
#include <QStringList>
#include <QToolBar>

constexpr char resourceDirectory[] = ":/module";

ModulesEditor::ModulesEditor(QWidget *parent) : QDialog(parent, Qt::Window)
                                              , slaveModel(nullptr)
                                              , masterModel(nullptr)
{
    if (parent != nullptr)
    {
        SetupUI(parent->size());
        ReadModulesToMasterModel();
        this->exec();
    }
}

void ModulesEditor::SetupUI(QSize pSize)
{
    this->setGeometry(0, 0, pSize.width(), pSize.height());
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
        //qtw->setHeaderLabel("Свойства");
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
    if (homeFiles.count() < xmlFiles.count()) {
        foreach (QString filename, xmlFiles) {
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
    masterModel = new QStandardItemModel(modules.count(), 4);
    masterModel->setHeaderData(0, Qt::Horizontal, "Название");
    masterModel->setHeaderData(1, Qt::Horizontal, "Type B");
    masterModel->setHeaderData(2, Qt::Horizontal, "Type M");
    masterModel->setHeaderData(3, Qt::Horizontal, "Версия");
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
                ParseXmlToMasterModel(domElement, i, masterModel);
            }
            moduleFile->close();
        }
        delete domDoc;
        delete moduleFile;
    }
}

void ModulesEditor::ParseXmlToMasterModel(const QDomNode &node, const int &index, QStandardItemModel *model)
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
                    model->setData(indexTypeM, domElement.attribute("mtypem", "00"));
                    model->setData(indexTypeB, domElement.attribute("mtypeb", "00"));
                }
                // Получаем имя модуля
                else if (domElement.tagName() == "name")
                {
                    auto indexName = masterModel->index(index, 0);
                    model->setData(indexName, domElement.text());
                }
                // И его версию
                else if (domElement.tagName() == "version")
                {
                    auto indexVersion = masterModel->index(index, 3);
                    model->setData(indexVersion, domElement.text());
                    break;
                }
            }
        }
        // Делаем это всё рекурсивно
        ParseXmlToMasterModel(domNode, index, model);
        domNode = domNode.nextSibling();
    }
}

void ModulesEditor::MasterItemSelected(const QModelIndex &index)
{
    const auto row = index.row();
    auto indexTypeB = masterModel->index(row, 1),
         indexTypeM = masterModel->index(row, 2);
    auto dataTypeB = masterModel->data(indexTypeB),
         dataTypeM = masterModel->data(indexTypeM);
    if (dataTypeB.canConvert<QString>() && dataTypeM.canConvert<QString>())
    {
        auto moduleType = (qvariant_cast<QString>(dataTypeB) + qvariant_cast<QString>(dataTypeM)).toLower();
        QDir homeDir(StdFunc::GetSystemHomeDir());
        auto module = homeDir.entryList(QDir::Files).filter(moduleType)[0];
        qDebug() << module;

        //if (slaveModel == nullptr)
        //    slaveModel = CreateSlaveModel(0);

        auto domDoc = new QDomDocument;
        auto moduleFile = new QFile(homeDir.filePath(module));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            if (domDoc->setContent(moduleFile))
            {
                auto domElement = domDoc->documentElement();
                //SearchModule(domElement, i, masterModel);
            }
            moduleFile->close();
        }
        delete domDoc;
        delete moduleFile;
    }

}

