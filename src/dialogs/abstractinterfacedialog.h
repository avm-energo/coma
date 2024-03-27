#pragma once

#include <QDialog>
#include <QSettings>
#include <QTableView>
#include <interfaces/types/settingstypes.h>

class AbstractInterfaceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AbstractInterfaceDialog(QWidget *parent = nullptr);
    virtual void setupUI();

    virtual bool updateModel() = 0;

protected:
    QSettings m_settings;
    QTableView *m_tableView;

    virtual void acceptedInterface() {};
    virtual void setInterface(QModelIndex index) = 0;
    virtual void addInterface()
    {
    }

    void removeConnection(const QString &connName);
    bool isConnectionExist(const QString &connName);
    bool checkSize();

signals:
    void accepted(const ConnectStruct &st);
};
