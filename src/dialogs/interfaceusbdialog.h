#pragma once

#include "abstractinterfacedialog.h"

class InterfaceUSBDialog final : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    explicit InterfaceUSBDialog(QWidget *parent);

    void setupUI() override;
    bool updateModel() override;

private:
    void setInterface(QModelIndex index) override;
};
