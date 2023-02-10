#pragma once

#include "xmldialog.h"

class XmlSignalDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlSignalDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
