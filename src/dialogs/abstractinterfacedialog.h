#pragma once

#include "../interfaces/settingstypes.h"

#include <QDialog>
#include <QTableView>

class AbstractInterfaceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AbstractInterfaceDialog(QWidget *parent = nullptr);
    virtual void setupUI();

    virtual bool updateModel() = 0;

protected:
    virtual void acceptedInterface() {};
    virtual void setInterface(QModelIndex index) = 0;
    virtual void addInterface()
    {
    }
    virtual void scanInterface()
    {
    }

    void rotateSettings(const QString &type, const QString &name);
    bool isKeyExist(const QString &type, const QString &chstr);
    void removeDevice(const QString &name);

    QTableView *tableView;
signals:
    void accepted(ConnectStruct);
};
