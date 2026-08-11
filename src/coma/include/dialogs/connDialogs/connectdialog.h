#pragma once

#include <dialogs/connDialogs/abstractinterfacedialog.h>

#include <QSettings>
#include <QWidget>

class ConnectDialog final : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget *parent = nullptr);

signals:
    void accepted(const ConnectionSettings &st);

protected:
    void hideEvent(QHideEvent *event) override;

private slots:
    void setInterface(const QString &connectionType);

private:
    AbstractInterfaceDialog *m_idialog;
    QSettings settings;
};
