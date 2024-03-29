#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

class XmlBsiExtDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlBsiExtDialog(QWidget *parent = nullptr);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
