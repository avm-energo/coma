#pragma once

#include <QProgressDialog>
#include <dialogs/abstractinterfacedialog.h>

class InterfaceEmuDialog final : public AbstractInterfaceDialog
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
