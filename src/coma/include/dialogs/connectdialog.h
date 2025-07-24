#pragma once

#include <QDialog>
#include <QSettings>
#include <dialogs/abstractinterfacedialog.h>

class ConnectDialog final : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget *parent = nullptr);

signals:
    void accepted(const ConnectStruct &st);

private slots:
    void setInterface();

private:
    AbstractInterfaceDialog *m_idialog;
    QSettings settings;
};
