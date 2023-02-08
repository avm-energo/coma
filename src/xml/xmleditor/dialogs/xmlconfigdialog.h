#pragma once

#include "xmldialog.h"

class XmlConfigDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlConfigDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
