#include "moduleseditor.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"

#include <QPushButton>
#include <QStringList>
#include <QToolBar>

constexpr char resourceDirectory[] = ":/module";

ModulesEditor::ModulesEditor(QWidget *parent) : QWidget(parent)
{
    if (parent != nullptr) {
        auto size = parent->size();
        const int margin = 30;
        this->setGeometry(-4, margin, size.width(), size.height() - margin * 2 - 20);
        SetupUI(size);
        ReadModulesToModel();
    }
}

void ModulesEditor::SetupUI(QSize& pSize)
{
    auto mainLayout = new QVBoxLayout(this);
    auto workspacesLayout = new QHBoxLayout;
    auto cBtnLayout = new QHBoxLayout;

    // Настройка кнопки закрытия
    auto closeButton = new QPushButton(this);
    closeButton->setIcon(QIcon(":/icons/tnstop.svg"));
    closeButton->setIconSize(QSize(40, 40));
    QObject::connect(closeButton, &QPushButton::clicked, this, &ModulesEditor::Close);
    cBtnLayout->addSpacing(pSize.width() - (40 + 10));
    cBtnLayout->addWidget(closeButton);
    cBtnLayout->addSpacing(10);

    // Получение рабочих пространств
    auto master = GetWorkspace(WorkspaceType::Master);
    auto slave = GetWorkspace(WorkspaceType::Slave);
    workspacesLayout->addLayout(master);
    workspacesLayout->addLayout(slave);
    mainLayout->addLayout(cBtnLayout);
    mainLayout->addLayout(workspacesLayout);
    this->setLayout(mainLayout);
    this->show();
}

void ModulesEditor::Close()
{
    this->hide();
}

QVBoxLayout *ModulesEditor::GetWorkspace(WorkspaceType type)
{
    // Создание рабочего пространства
    auto workspace = new QVBoxLayout();
    workspace->setSpacing(15);
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

void ModulesEditor::ReadModulesToModel()
{
    auto dir = UnpackData();
    auto modules = dir.entryList(QDir::Files).filter(".xml");
    masterModel = new QStandardItemModel(modules.count(), 4);
    masterModel->setHeaderData(0, Qt::Horizontal, "Название");
    masterModel->setHeaderData(1, Qt::Horizontal, "Type B");
    masterModel->setHeaderData(2, Qt::Horizontal, "Type M");
    masterModel->setHeaderData(3, Qt::Horizontal, "Версия");
    masterView->setModel(masterModel);

    for (int i = 0; i < modules.count(); i++)
    {

        auto domDoc = new QDomDocument;
        auto moduleFile = new QFile(dir.filePath(modules[i]));
        if (moduleFile->open(QIODevice::ReadOnly))
        {
            if (domDoc->setContent(moduleFile))
            {
                auto domElement = domDoc->documentElement();
                SearchModule(domElement, i, masterModel);
            }
            moduleFile->close();
        }

        delete domDoc;
        delete moduleFile;
    }
}

void ModulesEditor::SearchModule(const QDomNode &node, const int &index, QStandardItemModel *model)
{
    auto domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            auto domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "module")
                {
                    auto indexTypeB = masterModel->index(index, 1);
                    auto indexTypeM = masterModel->index(index, 2);
                    model->setData(indexTypeM, domElement.attribute("mtypem", ""));
                    model->setData(indexTypeB, domElement.attribute("mtypeb", ""));
                }
                else if (domElement.tagName() == "name")
                {
                    auto indexName = masterModel->index(index, 0);
                    model->setData(indexName, domElement.text());
                }
                else if (domElement.tagName() == "version")
                {
                    auto indexVersion = masterModel->index(index, 3);
                    model->setData(indexVersion, domElement.text());
                    break;
                }
            }
        }
        SearchModule(domNode, index, model);
        domNode = domNode.nextSibling();
    }
}

