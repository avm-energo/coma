#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include "abstractinterfacedialog.h"

#include <QDialog>
#include <QSettings>

class ConnectDialog : public QDialog
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

#endif // CONNECTDIALOG_H
