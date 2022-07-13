#pragma once

#include "../widgets/udialog.h"

#include <QWidget>

class SignalDialog : public UDialog
{
    Q_OBJECT
public:
    explicit SignalDialog(QWidget *parent = nullptr);

signals:
};
