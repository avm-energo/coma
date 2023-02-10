#pragma once

#include "xmldialog.h"

class XmlAlarmDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlAlarmDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
