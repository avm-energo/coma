#include "xmleditor.h"

#include "../../widgets/epopup.h"
#include "dialogs/dialogfabric.h"
#include "models/modelfabric.h"

#include <QFont>
#include <QHeaderView>
#include <QLabel>
#include <QToolBar>

constexpr auto iconSize = 40;
constexpr auto contentMargin = 5;

XmlEditor::XmlEditor(QWidget *parent) : QDialog(parent, Qt::Window), dc(nullptr), masterModel(nullptr), manager(nullptr)
{
    if (parent != nullptr)
    {
        dc = new DataController(this);
        QObject::connect(dc, &DataController::highlightModified, //
            [this]() {
                auto row = masterView->selectionModel()->selectedRows().at(0).row();
                dc->setRow(row);
                setFontBolding(row, true);
            });
        manager = new ModelManager(this);
        QObject::connect(manager, &ModelManager::SaveModule, this, &XmlEditor::saveModule);
        setupUI(parent->size());
        exec();
    }
}

/// \brief Настройка интерфейса диалогового окна редактора.
void XmlEditor::setupUI(QSize pSize)
{
    // Размер окна
    this->setGeometry(0, 0, pSize.width(), pSize.height());
    this->setWindowTitle("Modules Editor");

    // Добавление рабочих пространств на основной слой
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(getMasterWorkspace());
    mainLayout->addLayout(getSlaveWorkspace());
    setLayout(mainLayout);
}

/// \brief Действия, выполняемые при закрытии окна редактора.
void XmlEditor::reject()
{
    saveModule();
    hide();
}

/// \brief Возвращает рабочее пространство master (левая часть окна).
QVBoxLayout *XmlEditor::getMasterWorkspace()
{
    auto workspace = new QVBoxLayout;
    workspace->setContentsMargins(5, 5, 5, 5);

    // Настройка тулбара
    masterView = new QTableView(this);
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(iconSize, iconSize));
    toolbar->addAction(QIcon(":/icons/createModule.svg"), "Создать модуль", this,     //
        [this]() { actionDialog(DialogType::Create, masterView); });                  //
    toolbar->addSeparator();                                                          //
    toolbar->addAction(QIcon(":/icons/editModule.svg"), "Редактировать модуль", this, //
        [this]() { actionDialog(DialogType::Edit, masterView); });                    //
    toolbar->addSeparator();                                                          //
    toolbar->addAction(QIcon(":/icons/deleteModule.svg"), "Удалить модуль", this,     //
        [this]() { actionDialog(DialogType::Remove, masterView); });                  //
    toolbar->addSeparator();                                                          //
    toolbar->addAction(QIcon(":/icons/tnsave.svg"), "Сохранить модуль", this, &XmlEditor::saveModule);
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
    // Подключение контроллера данных
    QObject::connect(masterModel, &MasterModel::createFile, dc, &DataController::createFile);
    QObject::connect(masterModel, &MasterModel::removeFile, dc, &DataController::removeFile);
    QObject::connect(masterModel, &MasterModel::modelChanged, dc, &DataController::configChanged);
    masterView->setModel(masterModel);
    return workspace;
}

/// \brief Возвращает рабочее пространство slave (правая часть окна).
QVBoxLayout *XmlEditor::getSlaveWorkspace()
{
    auto workspace = new QVBoxLayout;
    workspace->setContentsMargins(contentMargin, contentMargin, contentMargin, contentMargin);

    // Настройка тулбара
    tableSlaveView = new QTableView(this);
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(iconSize, iconSize));
    toolbar->addAction(QIcon(":/icons/createModule.svg"), "Создать", this,     //
        [this]() { actionDialog(DialogType::Create, tableSlaveView); });       //
    toolbar->addSeparator();                                                   //
    toolbar->addAction(QIcon(":/icons/editModule.svg"), "Редактировать", this, //
        [this]() { actionDialog(DialogType::Edit, tableSlaveView); });         //
    toolbar->addSeparator();                                                   //
    toolbar->addAction(QIcon(":/icons/deleteModule.svg"), "Удалить", this,     //
        [this]() { actionDialog(DialogType::Remove, tableSlaveView); });       //
    workspace->addWidget(toolbar);                                             //

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
        [this](XmlModel *model) {
            tableSlaveView->setModel(model);
            tableSlaveView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
            QObject::connect(model, &XmlModel::modelChanged, dc, &DataController::configChanged);
        });
    QObject::connect(manager, &ModelManager::EditQuery, this,          //
        [this]() { actionDialog(DialogType::Edit, tableSlaveView); }); //
    workspace->addWidget(tableSlaveView);
    return workspace;
}

/// \brief Слот для создания диалогового окна создания, редактирования или удаления выбранного элемента.
void XmlEditor::actionDialog(DialogType dlgType, QTableView *srcView)
{
    auto model = qobject_cast<BaseEditorModel *>(srcView->model());
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

/// \brief Слот для смены bolding текста элемента, который был изменён (для master workspace).
void XmlEditor::setFontBolding(int row, bool state)
{
    auto cols = masterModel->columnCount();
    for (auto column = 0; column < cols; column++)
    {
        auto index = masterModel->index(row, column);
        auto font = masterModel->data(index, Qt::FontRole).value<QFont>();
        font.setBold(state);
        masterModel->setData(index, font, Qt::FontRole);
    }
}

/// \brief Слот для создания диалогового окна о сохранении изменений в выбранном модуле.
void XmlEditor::savingAsk()
{
    auto slaveModel = manager->GetRootModel();
    if (EMessageBox::question(this, "Сохранить изменения?"))
    {
        if (slaveModel != nullptr)
            dc->saveFile(masterModel, slaveModel);
        else
            dc->saveFile(masterModel);
    }
    else
    {
        auto isModuleOpened = false;
        if (slaveModel != nullptr)
            isModuleOpened = true;
        masterModel->undoChanges(dc->getRow(), isModuleOpened);
    }
}

/// \brief Слот для сохранения или сброса изменений настроек модуля.
void XmlEditor::saveModule()
{
    if (dc->getModuleState())
    {
        dc->resetOrSaved();
        savingAsk();
        setFontBolding(dc->getRow(), false);
    }
}
