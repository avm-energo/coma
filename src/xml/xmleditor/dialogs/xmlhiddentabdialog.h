#pragma once

#include "xmldialog.h"

class XmlHiddenTabDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlHiddenTabDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
