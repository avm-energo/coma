#pragma once

#include "xmldialog.h"

class XmlHiddenWidgetDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlHiddenWidgetDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
