#pragma once
#include "abstractinterfacedialog.h"

class InterfaceSerialDialog : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    explicit InterfaceSerialDialog(QWidget *parent = nullptr);

    void setupUI() override;
    void setInterface(QModelIndex index) override;
    void addInterface() override;
    bool updateModel(QDialog *dlg) override;
    void acceptedInterface() override;
};
