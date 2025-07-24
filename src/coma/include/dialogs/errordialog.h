#pragma once

#include <QDialog>

class ErrorDialog final : public QDialog
{
    Q_OBJECT
public:
    explicit ErrorDialog(QWidget *parent = nullptr);

private:
    void SetupUI();
};
