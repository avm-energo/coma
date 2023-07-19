#pragma once
#include "abstractinterfacedialog.h"

#include <QSettings>

class InterfaceSerialDialog : public AbstractInterfaceDialog
{
    Q_OBJECT
private:
    QSettings settings;

public:
    explicit InterfaceSerialDialog(QWidget *parent = nullptr);
    ~InterfaceSerialDialog() noexcept;
    void setupUI() override;
    bool updateModel() override;

private:
    void setInterface(QModelIndex index) override;
    void addInterface() override;
    void acceptedInterface() override;

    void editConnection(QModelIndex index);
    void removeConnection(const QString &name);
    bool isNameExist(const QString &name);
    bool checkSize();
};
