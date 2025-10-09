#pragma once

#include <dialogs/abstractinterfacedialog.h>

#include <QDialog>
#include <QSettings>

class ConnectDialog final : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget *parent = nullptr);

signals:
    void accepted(const ConnectionSettings &st);

private slots:
    void setInterface();

private:
    AbstractInterfaceDialog *m_idialog;
    QSettings settings;
};
