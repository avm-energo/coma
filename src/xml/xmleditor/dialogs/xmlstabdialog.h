#pragma once

#include "xmldialog.h"

class XmlSTabDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlSTabDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
