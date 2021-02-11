#pragma once
#include "abstractinterfacedialog.h"

class InterfaceSerialDialog : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    explicit InterfaceSerialDialog(QWidget *parent = nullptr);

    void setupUI() override;
    bool updateModel() override;

private:
    void setInterface(QModelIndex index) override;
    void addInterface() override;

    void acceptedInterface() override;
};
