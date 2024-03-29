#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

class XmlHiddenWidgetDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlHiddenWidgetDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
