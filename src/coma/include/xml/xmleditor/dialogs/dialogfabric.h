#pragma once

#include <xml/xmleditor/models/baseeditormodel.h>

/// \brief Класс со статическими методами для создания диалоговых окон.
class XmlDialogFabric
{
private:
    /// \brief Диалог создания или изменения элемента.
    static void createOrEditDialog(BaseEditorModel *model, int row, QWidget *parent);
    /// \brief Диалог удаления или изменения элемента.
    static void removeOrEditDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent, int type);

public:
    XmlDialogFabric() = delete;

    /// \brief Создать и отобразить диалог создания нового элемента.
    static void createDialog(BaseEditorModel *model, QWidget *parent = nullptr);
    /// \brief Создать и отобразить диалог редактирования выбранного элемента.
    static void editDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
    /// \brief Создать и отобразить диалог удаления выбранного элемента.
    static void removeDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
};
