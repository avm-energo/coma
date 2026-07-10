#pragma once

#include <dialogs/abstractinterfacedialog.h>

class InterfaceSerialDialog final : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    explicit InterfaceSerialDialog(QWidget *parent = nullptr);
    ~InterfaceSerialDialog() noexcept;
    void setupUI() override;
    bool updateModel() override;
    QStringList getConnectionNames() const;
    void removeConnection(const QString &name);
    void addConnectionFromSearch(const QMap<QString, QVariant> &deviceData);

private:
    void setInterface(QModelIndex index) override;
    void addInterface() override;
    void acceptedInterface() override;
    void editConnection(QModelIndex index);
};
