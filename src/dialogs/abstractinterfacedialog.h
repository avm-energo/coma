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
    virtual void setInterface(QModelIndex index) = 0;
    virtual void addInterface()
    {
    }
    virtual void scanInterface()
    {
    }
    virtual bool updateModel(QDialog *dlg) = 0;

protected:
    virtual void acceptedInterface() {

    };

    void rotateSettings(const QString &type, const QString &name);
    bool isKeyExist(const QString &type, const QString &chstr);
signals:
    void accepted(ConnectStruct);

protected:
    QTableView *tableView;
};
