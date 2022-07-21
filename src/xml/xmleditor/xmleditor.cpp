#include "xmleditor.h"

#include "dialogs/xmldialogs.h"

#include <QHeaderView>
#include <QLabel>
#include <QToolBar>

XmlEditor::XmlEditor(QWidget *parent) : QDialog(parent, Qt::Window), masterModel(nullptr), manager(nullptr)
{
    if (parent != nullptr)
    {
        manager = new ModelManager(this);
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
    mainLayout->addLayout(GetMasterWorkspace());
    mainLayout->addLayout(GetSlaveWorkspace());
    this->setLayout(mainLayout);
}

void XmlEditor::Close()
{
    this->hide();
}

QVBoxLayout *XmlEditor::GetMasterWorkspace()
{
    auto workspace = new QVBoxLayout(this);
    workspace->setContentsMargins(5, 5, 5, 5);

    // Настройка тулбара
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(30, 30));
    toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать модуль", this, &XmlEditor::Close);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить модуль", this, &XmlEditor::Close);
    workspace->addWidget(toolbar);

    // Создание и настройка QTableView для master
    masterView = new QTableView(this);
    masterView->setSortingEnabled(true);
    masterView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    masterView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    masterView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto header = masterView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    workspace->addWidget(masterView);

    return workspace;
}

QVBoxLayout *XmlEditor::GetSlaveWorkspace()
{
    auto workspace = new QVBoxLayout(this);
    workspace->setContentsMargins(5, 5, 5, 5);

    // Настройка тулбара
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(30, 30));
    toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать", this, &XmlEditor::Close);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnosc.svg"), "Редактировать", this, &XmlEditor::EditItem);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить", this, &XmlEditor::Close);
    workspace->addWidget(toolbar);

    // Label для отображения текущего положения в дереве моделей
    auto curPath = new QLabel("", this);
    QObject::connect(manager, &ModelManager::PathChanged, curPath, &QLabel::setText);
    workspace->addWidget(curPath);

    // Создание и настройка QTableView для slave
    tableSlaveView = new QTableView(this);
    tableSlaveView->setSortingEnabled(true);
    tableSlaveView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    tableSlaveView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    tableSlaveView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto header = tableSlaveView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    QObject::connect(tableSlaveView, &QTableView::doubleClicked, manager, &ModelManager::ViewModelItemClicked);
    QObject::connect(manager, &ModelManager::ModelChanged, this, //
        [&](XmlSortProxyModel *model) -> void {
            tableSlaveView->setModel(model);
            tableSlaveView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
        });
    workspace->addWidget(tableSlaveView);

    return workspace;
}

void XmlEditor::CreateMasterModel()
{
    if (masterModel == nullptr)
    {
        masterModel = new MasterModel(this);
        masterModel->setHorizontalHeaderLabels({ "Устройство", "Type B", "Type M", "Версия", "Файл" });
        QObject::connect(masterView, &QTableView::doubleClicked, masterModel, &MasterModel::masterItemSelected);
        QObject::connect(masterModel, &MasterModel::itemSelected, manager, &ModelManager::SetDocument);
    }
    masterView->setModel(masterModel);
}

void XmlEditor::EditItem()
{
    auto proxyModel = qobject_cast<XmlSortProxyModel *>(tableSlaveView->model());
    auto model = qobject_cast<XmlModel *>(proxyModel->sourceModel());
    auto type = model->getModelType();
    auto selected = tableSlaveView->selectionModel()->selectedRows();
    if (!selected.isEmpty())
    {
        auto row = selected.at(0).row();
        XmlEditDialog *dialog = nullptr;
        if (row != 0)
        {
            switch (type)
            {
            case ModelType::AlarmsItem:
                // qDebug() << model->data(model->index(row, 0)) << model->data(model->index(row, 1));
                dialog = new XmlEditAlarmDialog(proxyModel, this);
                break;
            default:
                // TODO: What must happenes here?
                // Предположительно, ничего, как и сейчас
                // qDebug() << "Тута нельзя редактировать ничаво!";
                break;
            }
        }
        else
        {
            if (type == ModelType::Resources)
            {
                // TODO: Вызывать диалоговое окно для ModelType::Resources
            }
        }
        if (dialog != nullptr)
        {
            dialog->setupUICall(row);
        }
    }
    else
    {
        // TODO: Выводить сообщение, что ничего не выбрано
    }
}
