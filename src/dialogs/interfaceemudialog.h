#pragma once
#include "abstractinterfacedialog.h"

#include <QProgressDialog>

class InterfaceEmuDialog : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    explicit InterfaceEmuDialog(QWidget *parent = nullptr);
    ~InterfaceEmuDialog() noexcept;

    void setupUI() override;
    bool updateModel() override;

protected:
    void setInterface(QModelIndex index) override;
    void addInterface() override;

    void acceptedInterface() override;
};
