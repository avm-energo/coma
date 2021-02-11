#pragma once
#include "abstractinterfacedialog.h"

class InterfaceUSBDialog : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    explicit InterfaceUSBDialog(QWidget *parent);

    void setupUI() override;
    void setInterface(QModelIndex index) override;
    bool updateModel(QDialog *dlg) override;
};
