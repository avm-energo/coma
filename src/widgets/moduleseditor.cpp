#include "moduleseditor.h"

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
        const int someMagic = 80;
        this->setGeometry(0, someMagic, size.width(), size.height() - (someMagic + 50));
    }
    SetupUI();
}

void ModulesEditor::SetupUI()
{
    auto hLayout = new QHBoxLayout;
    // TODO: доделать UI

    master = GetWorkspace(WorkspaceType::Master);
    slave = GetWorkspace(WorkspaceType::Slave);

    hLayout->addLayout(master);
    hLayout->addLayout(slave);

    this->setLayout(hLayout);
}

void ModulesEditor::Close()
{
    this->hide();
}

QVBoxLayout *ModulesEditor::GetWorkspace(WorkspaceType type)
{
    // Создание рабочего пространства
    auto workspace = new QVBoxLayout;
    workspace->setSpacing(15);
    workspace->setContentsMargins(5, 5, 5, 5);

    // Настройка QTreeWidget
    auto qtw = new QTreeWidget();
    if (type == Master)
        qtw->setHeaderLabels({"Название", "Версия", "Type B", "Type M"});
    else
        qtw->setHeaderLabel("Свойства");
    qtw->setSortingEnabled(true);
    workspace->addWidget(qtw);
    workspace->addWidget(new QSplitter());

    // Настройка тулбара
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(40, 40));
    toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Создать/добавить", this, &ModulesEditor::Close);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Удалить", this, &ModulesEditor::Close);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnsettings.svg"), "Закрыть", this, &ModulesEditor::Close);
    workspace->addWidget(toolbar);

    return workspace;
}
