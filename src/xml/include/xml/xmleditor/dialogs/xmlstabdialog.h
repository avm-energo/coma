#pragma once

#include "xmldialog.h"

class XmlSTabDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlSTabDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
