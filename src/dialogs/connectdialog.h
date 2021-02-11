#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include "abstractinterfacedialog.h"

#include <QDialog>

class ConnectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget *parent = nullptr);

signals:
    void accepted(ConnectStruct);

private slots:
    void setInterface();

private:
    AbstractInterfaceDialog *m_idialog;
};

#endif // CONNECTDIALOG_H
