#pragma once

#include "xmldialog.h"

class XmlSectionDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlSectionDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
