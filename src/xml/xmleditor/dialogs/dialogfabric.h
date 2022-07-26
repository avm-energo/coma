#ifndef XMLDIALOGFABRIC_H
#define XMLDIALOGFABRIC_H

#include "../models/xmlsortproxymodel.h"

class XmlDialogFabric
{
private:
    static void Dialog(XmlSortProxyModel *model, int row, QWidget *parent);
    static void Dialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent, int type);

public:
    explicit XmlDialogFabric() = delete;

    static void CreateDialog(XmlSortProxyModel *model, QWidget *parent = nullptr);
    static void EditDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
    static void RemoveDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent = nullptr);
};

#endif // XMLDIALOGFABRIC_H
