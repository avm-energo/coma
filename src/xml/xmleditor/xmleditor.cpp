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
    auto workspace = new QVBoxLayout;
    workspace->setContentsMargins(5, 5, 5, 5);

    // Настройка тулбара
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(30, 30));
    toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать модуль", this, &XmlEditor::CreateModule);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnosc.svg"), "Редактировать модуль", this, &XmlEditor::EditModule);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить модуль", this, &XmlEditor::RemoveModule);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnsave.svg"), "Сохранить модуль", this, &XmlEditor::SaveModule);
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
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(30, 30));
    toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать", this,     //
        [&]() { SlaveModelDialog(DialogType::Create); });                 //
    toolbar->addSeparator();                                              //
    toolbar->addAction(QIcon(":/icons/tnosc.svg"), "Редактировать", this, //
        [&]() { SlaveModelDialog(DialogType::Edit); });                   //
    toolbar->addSeparator();                                              //
    toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить", this,      //
        [&]() { SlaveModelDialog(DialogType::Remove); });                 //
    workspace->addWidget(toolbar);                                        //

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
        [&](XmlSortProxyModel *model) {
            tableSlaveView->setModel(model);
            tableSlaveView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
        });
    QObject::connect(manager, &ModelManager::EditQuery, this, //
        [&]() { SlaveModelDialog(DialogType::Edit); });       //
    workspace->addWidget(tableSlaveView);

    return workspace;
}

void XmlEditor::SlaveModelDialog(DialogType dlgType)
{
    auto proxyModel = qobject_cast<XmlSortProxyModel *>(tableSlaveView->model());
    switch (dlgType)
    {
    // Диалог создания элемента
    case DialogType::Create:
        if (proxyModel != nullptr)
            XmlDialogFabric::CreateDialog(proxyModel, this);
        break;
    // Диалоги редактирования или удаления элементов
    case DialogType::Edit:
    case DialogType::Remove:
        auto selectModel = tableSlaveView->selectionModel();
        if (selectModel != nullptr)
        {
            auto selected = selectModel->selectedRows();
            // Диалог редактирования элемента
            if (dlgType == DialogType::Edit)
                XmlDialogFabric::EditDialog(proxyModel, selected, this);
            // Диалог удаления элемента
            else
                XmlDialogFabric::RemoveDialog(proxyModel, selected, this);
        }
        break;
    }
}

void XmlEditor::CreateModule()
{
    auto moduleDlg = new ModuleDialog(this);
    QObject::connect(moduleDlg, &ModuleDialog::ModelDataRequest, masterModel, &MasterModel::getDialogRequest);
    QObject::connect(masterModel, &MasterModel::sendDialogResponse, moduleDlg, &ModuleDialog::ModelDataResponse);
    QObject::connect(moduleDlg, &ModuleDialog::CreateData, masterModel, &MasterModel::create);
    QObject::connect(moduleDlg, &ModuleDialog::UpdateData, masterModel, &MasterModel::update);
    moduleDlg->SetData();
    moduleDlg->exec();
}

void XmlEditor::EditModule()
{
    auto moduleDlg = new ModuleDialog(this);
    QObject::connect(moduleDlg, &ModuleDialog::ModelDataRequest, masterModel, &MasterModel::getDialogRequest);
    QObject::connect(masterModel, &MasterModel::sendDialogResponse, moduleDlg, &ModuleDialog::ModelDataResponse);
    QObject::connect(moduleDlg, &ModuleDialog::CreateData, masterModel, &MasterModel::create);
    QObject::connect(moduleDlg, &ModuleDialog::UpdateData, masterModel, &MasterModel::update);
    auto selected = masterView->selectionModel()->selectedRows()[0].row();
    moduleDlg->SetData(selected);
    moduleDlg->exec();
}

void XmlEditor::RemoveModule()
{
    auto selected = masterView->selectionModel()->selectedRows()[0].row();
    masterModel->remove(selected);
}

void XmlEditor::SaveModule()
{
    ;
    ;
}
