#ifndef XMLDIALOGFABRIC_H
#define XMLDIALOGFABRIC_H

#include "../models/ieditormodel.h"

/// \brief Класс со статическими методами для создания диалоговых окон.
class XmlDialogFabric
{
private:
    static void CreateOrEditDialog(IEditorModel *model, int row, QWidget *parent);
    static void RemoveOrEditDialog(IEditorModel *model, QModelIndexList &selected, QWidget *parent, int type);

public:
    explicit XmlDialogFabric() = delete;
    static void CreateDialog(IEditorModel *model, QWidget *parent = nullptr);
    static void EditDialog(IEditorModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
    static void RemoveDialog(IEditorModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
};

#endif // XMLDIALOGFABRIC_H
