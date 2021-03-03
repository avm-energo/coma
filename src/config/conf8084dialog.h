#pragma once

#include "confkivdialog.h"

class Conf8084Dialog : public ConfKIVDialog
{
    Q_OBJECT
public:
    explicit Conf8084Dialog(ConfigKIV *ckiv, QWidget *parent = nullptr);

private:
    void SetupUI() override;
    void Fill() override;
    void FillBack() override;

    virtual QWidget *connectionWidget() override;
signals:
};
