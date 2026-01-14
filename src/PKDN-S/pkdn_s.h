#pragma once

#include "../gen/mainwindow.h"

#include <QResizeEvent>

class pkdn_s : public MainWindow
{
    Q_OBJECT

public:
    pkdn_s(QWidget *parent = 0);
    void SetupUI();
    void AddActionsToMenuBar(QMenuBar *menubar);
    void Stage3();

signals:
    void VoltageTypeChanged(int mode);

private:
    QDialog *ChangeVoltageTypeDialog();

private slots:
    void AcceptVoltageType();
};
