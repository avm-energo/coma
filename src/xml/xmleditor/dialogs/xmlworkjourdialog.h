#pragma once

#include "xmldialog.h"

class XmlWorkJourDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlWorkJourDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
