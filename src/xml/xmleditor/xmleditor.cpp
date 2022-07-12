#include "xmleditor.h"

#include <QToolBar>

XmlEditor::XmlEditor(QWidget *parent) : QDialog(parent, Qt::Window), masterModel(nullptr)
{
    if (parent != nullptr)
    {
        SetupUI(parent->size());
        CreateMasterModel();
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
        masterView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        masterView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        workspace->addWidget(masterView);
    }
    else
    {
        // Создание и настройка QTableView для slave
        tableSlaveView = new QTableView(this);
        tableSlaveView->setSortingEnabled(true);
        tableSlaveView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        tableSlaveView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        // Создание и настройка QStackedWidget
        stackWidget = new QStackedWidget(this);
        stackWidget->addWidget(tableSlaveView);
        stackWidget->setCurrentWidget(tableSlaveView);
        workspace->addWidget(stackWidget);
    }

    return workspace;
}

void XmlEditor::CreateMasterModel()
{
    if (masterModel == nullptr)
    {
        masterModel = new MasterModel(this);
        masterModel->setSlaveView(tableSlaveView);
        masterModel->setHorizontalHeaderLabels({ "Файл", "Устройство", "Type B", "Type M", "Версия" });
        QObject::connect(masterView, &QTableView::clicked, masterModel, &MasterModel::masterItemSelected);
    }
    masterView->setModel(masterModel);
}
