#include "xmleditor.h"

#include "dialogs/dialogfabric.h"
#include "dialogs/moduledialog.h"
#include "models/modelfabric.h"

#include <QHeaderView>
#include <QLabel>
#include <QToolBar>

XmlEditor::XmlEditor(QWidget *parent) : QDialog(parent, Qt::Window), masterModel(nullptr), manager(nullptr)
{
    if (parent != nullptr)
    {
        manager = new ModelManager(this);
        SetupUI(parent->size());
        this->exec();
    }
}

void XmlEditor::SetupUI(QSize pSize)
{
    // Размер окна
    this->setGeometry(0, 0, pSize.width(), pSize.height());
    this->setWindowTitle("Modules Editor");

    // Добавление рабочих пространств на основной слой
    auto mainLayout = new QHBoxLayout(this);
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
    auto workspace = new QVBoxLayout;
    workspace->setContentsMargins(5, 5, 5, 5);

    // Настройка тулбара
    masterView = new QTableView(this);
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(30, 30));
    toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать модуль", this,     //
        [&]() { ActionDialog(DialogType::Create, masterView); });                //
    toolbar->addSeparator();                                                     //
    toolbar->addAction(QIcon(":/icons/tnosc.svg"), "Редактировать модуль", this, //
        [&]() { ActionDialog(DialogType::Edit, masterView); });                  //
    toolbar->addSeparator();                                                     //
    toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить модуль", this,      //
        [&]() { ActionDialog(DialogType::Remove, masterView); });                //
    toolbar->addSeparator();                                                     //
    toolbar->addAction(QIcon(":/icons/tnsave.svg"), "Сохранить модуль", this, &XmlEditor::SaveModule);
    workspace->addWidget(toolbar);

    // Настройка QTableView для master
    masterView->setSortingEnabled(true);
    masterView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    masterView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    masterView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto header = masterView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    workspace->addWidget(masterView);

    // Создание и настройка мастер модели
    masterModel = ModelFabric::CreateMasterModel(this);
    QObject::connect(masterView, &QTableView::doubleClicked, masterModel, &MasterModel::masterItemSelected);
    QObject::connect(masterModel, &MasterModel::itemSelected, manager, &ModelManager::SetDocument);
    masterView->setModel(masterModel);

    return workspace;
}

QVBoxLayout *XmlEditor::GetSlaveWorkspace()
{
    auto workspace = new QVBoxLayout;
    workspace->setContentsMargins(5, 5, 5, 5);

    // Настройка тулбара
    tableSlaveView = new QTableView(this);
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(30, 30));
    toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать", this,     //
        [&]() { ActionDialog(DialogType::Create, tableSlaveView); });     //
    toolbar->addSeparator();                                              //
    toolbar->addAction(QIcon(":/icons/tnosc.svg"), "Редактировать", this, //
        [&]() { ActionDialog(DialogType::Edit, tableSlaveView); });       //
    toolbar->addSeparator();                                              //
    toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить", this,      //
        [&]() { ActionDialog(DialogType::Remove, tableSlaveView); });     //
    workspace->addWidget(toolbar);                                        //

    // Label для отображения текущего положения в дереве моделей
    auto curPath = new QLabel("", this);
    QObject::connect(manager, &ModelManager::PathChanged, curPath, &QLabel::setText);
    workspace->addWidget(curPath);

    // Настройка QTableView для slave
    tableSlaveView->setSortingEnabled(true);
    tableSlaveView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    tableSlaveView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    tableSlaveView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto header = tableSlaveView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    QObject::connect(tableSlaveView, &QTableView::doubleClicked, manager, &ModelManager::ViewModelItemClicked);
    QObject::connect(manager, &ModelManager::ModelChanged, this, //
        [&](XmlModel *model) {
            tableSlaveView->setModel(model);
            tableSlaveView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
        });
    QObject::connect(manager, &ModelManager::EditQuery, this,       //
        [&]() { ActionDialog(DialogType::Edit, tableSlaveView); }); //
    workspace->addWidget(tableSlaveView);

    return workspace;
}

void XmlEditor::ActionDialog(DialogType dlgType, QTableView *srcView)
{
    auto model = qobject_cast<IEditorModel *>(srcView->model());
    switch (dlgType)
    {
    // Диалог создания элемента
    case DialogType::Create:
        if (model != nullptr)
            XmlDialogFabric::CreateDialog(model, this);
        break;
    // Диалоги редактирования или удаления элементов
    case DialogType::Edit:
    case DialogType::Remove:
        auto selectModel = srcView->selectionModel();
        if (selectModel != nullptr)
        {
            auto selected = selectModel->selectedRows();
            // Диалог редактирования элемента
            if (dlgType == DialogType::Edit)
                XmlDialogFabric::EditDialog(model, selected, this);
            // Диалог удаления элемента
            else
                XmlDialogFabric::RemoveDialog(model, selected, this);
        }
        break;
    }
}

void XmlEditor::SaveModule()
{
    ;
    ;
}
