#pragma once

#include <QDialog>
#include <QSettings>
#include <QTableView>
#include <interfaces/types/settingstypes.h>
#include <settings/user_settings.h>

// how much entries can we have for interfaces of each type in registry
constexpr int MAXREGISTRYINTERFACECOUNT = 5;

class AbstractInterfaceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AbstractInterfaceDialog(QWidget *parent = nullptr);
    virtual void setupUI();

    virtual bool updateModel() = 0;

protected:
    Settings::UserSettings &m_settings;
    // QSettings m_settings;
    QTableView *m_tableView;

    virtual void acceptedInterface() {};
    virtual void setInterface(QModelIndex index) = 0;
    virtual void addInterface()
    {
    }

    bool checkSize();

signals:
    void accepted(const ConnectStruct &st);
};
