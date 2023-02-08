#pragma once

#include "xmldialog.h"

class XmlMeasJourDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlMeasJourDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
