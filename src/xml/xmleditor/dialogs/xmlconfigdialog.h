#pragma once

#include "xmldialog.h"

class XmlConfigDialog final : public XmlDialog
{
    Q_OBJECT
public:
    XmlConfigDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
