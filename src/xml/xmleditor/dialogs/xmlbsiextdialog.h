#pragma once

#include "xmldialog.h"

class XmlBsiExtDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlBsiExtDialog(QWidget *parent = nullptr);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
