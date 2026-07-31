#pragma once

#include <dialogs/abstractinterfacedialog.h>

#include <QMenu>
#include <QSettings>

class ConnectDialog final : public QMenu
{
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget *parent = nullptr);

signals:
    void accepted(const ConnectionSettings &st);

private slots:
    void setInterface(const QString &connectionType);

private:
    AbstractInterfaceDialog *m_idialog;
    QSettings settings;
};
