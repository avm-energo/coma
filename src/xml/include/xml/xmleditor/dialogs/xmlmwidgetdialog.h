#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

class XmlMWidgetDialog final : public XmlDialog
{
    Q_OBJECT
public:
    XmlMWidgetDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
