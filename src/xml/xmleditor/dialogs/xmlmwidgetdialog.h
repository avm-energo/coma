#pragma once

#include "xmldialog.h"

class XmlMWidgetDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlMWidgetDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
