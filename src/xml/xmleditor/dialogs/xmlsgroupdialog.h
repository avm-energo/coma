#pragma once

#include "xmldialog.h"

class XmlSGroupDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlSGroupDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
