#pragma once

#include "xmldialog.h"

class XmlAlarmStateAllDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlAlarmStateAllDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
