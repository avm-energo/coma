#pragma once

#include "xmldialog.h"

class XmlAlarmDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlAlarmDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
