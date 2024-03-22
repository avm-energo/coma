#pragma once

#include "xmldialog.h"

class XmlResDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlResDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
