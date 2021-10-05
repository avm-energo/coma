#pragma once
#include "abstractinterfacedialog.h"

#include <QProgressDialog>

class InterfaceEmuDialog : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    InterfaceEmuDialog(QWidget *parent = nullptr);

    void setupUI() override;
    bool updateModel() override;

protected:
    void setInterface(QModelIndex index) override;
    void addInterface() override;

    void acceptedInterface() override;
};
