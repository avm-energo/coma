#include "moduleseditor.h"

#include <QPushButton>
#include <QSplitter>
#include <QStringList>
#include <QToolBar>
#include <QTreeView>
#include <QTreeWidget>

#include "wd_func.h"

ModulesEditor::ModulesEditor(QWidget *parent) : QWidget(parent)
                                              , master(nullptr)
                                              , slave(nullptr)
{
    if (parent != nullptr) {
        auto size = parent->size();
        const int margin = 30;
        this->setGeometry(-4, margin, size.width(), size.height() - margin * 2 - 20);
        SetupUI(size);
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
    master = GetWorkspace(WorkspaceType::Master);
    slave = GetWorkspace(WorkspaceType::Slave);
    workspacesLayout->addLayout(master);
    workspacesLayout->addLayout(slave);
    mainLayout->addLayout(cBtnLayout);
    mainLayout->addLayout(workspacesLayout);
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
        toolbar->addSeparator();
        toolbar->addAction(QIcon(":/icons/tnsettings.svg"), "Закрыть модуль", this, &ModulesEditor::Close);
    }
    workspace->addWidget(toolbar);

    // Настройка QTreeWidget
    auto qtw = new QTreeWidget(this);
    if (type == Master)
        qtw->setHeaderLabels({"Название", "Версия", "Type M", "Type B"});
    else
        qtw->setHeaderLabel("Свойства");
    qtw->setSortingEnabled(true);
    //workspace->addWidget(new QSplitter(this));
    workspace->addWidget(qtw);

    return workspace;
}
