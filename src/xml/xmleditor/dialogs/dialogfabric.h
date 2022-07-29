#ifndef XMLDIALOGFABRIC_H
#define XMLDIALOGFABRIC_H

#include "../models/mastermodel.h"
#include "../models/xmlsortproxymodel.h"

class XmlDialogFabric
{
private:
    static void CreateOrEditDialog(XmlSortProxyModel *model, int row, QWidget *parent);
    static void RemoveOrEditDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent, int type);

public:
    explicit XmlDialogFabric() = delete;

    // Items в Slave View
    static void CreateDialog(XmlSortProxyModel *model, QWidget *parent = nullptr);
    static void EditDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
    static void RemoveDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent = nullptr);

    // Modules в Master View
    static void CreateModuleDialog(MasterModel *model);
    static void EditModuleDialog(MasterModel *model);
    static void RemoveModuleDialog(MasterModel *model);
};

#endif // XMLDIALOGFABRIC_H
