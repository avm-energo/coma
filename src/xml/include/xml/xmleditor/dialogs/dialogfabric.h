#pragma once

#include <xml/xmleditor/models/baseeditormodel.h>

/// \brief Класс со статическими методами для создания диалоговых окон.
class XmlDialogFabric
{
private:
    static void CreateOrEditDialog(BaseEditorModel *model, int row, QWidget *parent);
    static void RemoveOrEditDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent, int type);

public:
    explicit XmlDialogFabric() = delete;
    static void CreateDialog(BaseEditorModel *model, QWidget *parent = nullptr);
    static void EditDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
    static void RemoveDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
};
